#include "logger.h"

#include <fstream>
#include <memory>
#include <iomanip>
#include <iostream>
#include <set>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/device/null.hpp>

#include <cis1_cwu_protocol/protocol.h>

struct cis_info
{
    size_t pid;
    size_t ppid;
    std::optional<std::string> session_id;
};

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(
                    s.rbegin(),
                    s.rend(),
                    [](int ch)
                    {
                        return !std::isspace(ch);
                    }).base(),
            s.end());
}

struct logger_interface
{
    virtual ~logger_interface() = default;
    virtual void prepare(const cis_info& state) = 0;
    virtual void log(const std::string& s) = 0;
};

class file_logger
    : public logger_interface
{
public:
    enum log_parts
    {
        time = 0b0001,
        pid = 0b0010,
        ppid = 0b0100,
        session = 0b1000,
    };

    file_logger(
            const std::filesystem::path& filename,
            int parts,
            std::ios_base::openmode mode = std::ios_base::out)
        : file_(filename, mode)
        , parts_(static_cast<log_parts>(parts))
    {}

    void prepare(const cis_info& state) override
    {
        if(parts_ & log_parts::time)
        {
            write_time();
        }

        if(parts_ & log_parts::pid)
        {
            write_pid(state);
        }

        if(parts_ & log_parts::ppid)
        {
            write_ppid(state);
        }

        if(parts_ & log_parts::session)
        {
            write_session(state);
        }
    }

    void log(const std::string& s) override
    {
        file_ << s;
        file_.flush();
    }

    bool is_open()
    {
        return file_.is_open();
    }

private:
    inline static const char* const delimeter_ = " | ";
    std::ofstream file_;
    log_parts parts_;

    void write_time()
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        file_ << std::put_time(std::localtime(&time), "%Y-%m-%d-%H-%M-%S") << delimeter_;
    }

    void write_pid(const cis_info& state)
    {
        const size_t max_pid_width = 8;
        file_ << std::setw(max_pid_width) << state.pid << delimeter_;
    }

    void write_ppid(const cis_info& state)
    {
        const size_t max_ppid_width = 8;
        file_ << std::setw(max_ppid_width) << state.ppid << delimeter_;
    }

    void write_session(const cis_info& state)
    {
        const size_t max_session_width = 38;
        file_ << std::setw(max_session_width);

        if(!state.session_id)
        {
            file_ << "unknown";
        }
        else
        {
            file_ << *(state.session_id);
        }

        file_ << delimeter_;
    }
};

class remote_logger
    : public logger_interface
{
public:
    remote_logger(const std::shared_ptr<webui_session>& session)
        : remote_logger_(session)
        , tr_(session->make_transaction())
    {}

    void prepare(const cis_info& state) override
    {
        dto_.time = std::chrono::system_clock::now();
    }

    void log(const std::string& s) override
    {
        dto_.message = s;
        rtrim(dto_.message);

        tr_.send(dto_);
    }

private:
    std::shared_ptr<webui_session> remote_logger_;
    cis1::cwu::log_entry dto_;
    cis1::proto_utils::transaction tr_;
};

class combined_logger
    : public logger_interface
{
public:
    template <class... Loggers>
    combined_logger(Loggers... loggers)
        : loggers_({loggers...})
    {}

    void prepare(const cis_info& state) override
    {
        for(auto& logger : loggers_)
        {
            (*logger)->prepare(state);
        }
    }

    void log(const std::string& s) override
    {
        for(auto& logger : loggers_)
        {
            (*logger)->log(s);
        }
    }

private:
    std::set<std::shared_ptr<logger_interface>*> loggers_;
};

class null_logger
    : public logger_interface
{
public:
    void prepare(const cis_info& state) override
    {}

    void log(const std::string&) override
    {}
};

struct flushable_sink_tag
    : public boost::iostreams::sink_tag
    , public boost::iostreams::flushable_tag
{};

class logger_device
{
public:
    typedef char char_type;
    typedef flushable_sink_tag category;

    logger_device(logger_interface& logger)
        : logger_(logger)
    {}

    std::streamsize write(const char_type* s, std::streamsize n)
    {
        const auto size = buffer_.size();
        buffer_.resize(buffer_.size() + n);
        std::copy(s, s + n, buffer_.data() + size);

        return n;
    }
    bool flush()
    {
        if(!buffer_.empty())
        {
            logger_.log(buffer_);
        }
        buffer_.clear();
        return true;
    }
private:
    logger_interface& logger_;
    std::string buffer_;
};

struct logger
{
    logger()
        : combined_loggers(
                &basic_loggers.cis_file,
                &basic_loggers.session_file,
                &basic_loggers.remote)
        , cis_log_sink(
                std::make_unique<
                        boost::iostreams::stream<logger_device>>(
                                combined_loggers.cis))
        , session_log_sink(
                std::make_unique<
                        boost::iostreams::stream<logger_device>>(
                                combined_loggers.session))
        , tee_log_sink(
                std::make_unique<
                        boost::iostreams::stream<logger_device>>(
                                combined_loggers.tee))
        , remote_log_sink(
                std::make_unique<
                        boost::iostreams::stream<logger_device>>(
                                combined_loggers.remote))
    {}

    logger(const logger& other) = delete;
    logger(logger&& other) = delete;

    logger& operator=(const logger& other) = delete;
    logger& operator=(logger&& other) = delete;

    cis_info state;

    struct basic_loggers
    {
        basic_loggers()
            : remote(&null, [](auto*){})
            , cis_file(&null, [](auto*){})
            , session_file(&null, [](auto*){})
        {}

        null_logger null;

        std::shared_ptr<logger_interface> remote;

        std::shared_ptr<logger_interface> cis_file;

        std::shared_ptr<logger_interface> session_file;
    } basic_loggers;

    struct combined_loggers
    {
        combined_loggers(
                std::shared_ptr<logger_interface>* cis_file,
                std::shared_ptr<logger_interface>* session_file,
                std::shared_ptr<logger_interface>* remote)
            : cis{cis_file, remote}
            , session{session_file, remote}
            , tee{cis_file, session_file, remote}
            , remote{remote}
        {}

        combined_logger cis;

        combined_logger session;

        combined_logger tee;

        combined_logger remote;
    } combined_loggers;

    std::unique_ptr<std::ostream> cis_log_sink;

    std::unique_ptr<std::ostream> session_log_sink;

    std::unique_ptr<std::ostream> remote_log_sink;

    std::unique_ptr<std::ostream> tee_log_sink;
};

logger global_logger;

void init_webui_log(std::shared_ptr<webui_session> session)
{
    global_logger.basic_loggers.remote = std::make_unique<remote_logger>(session);
}

void init_cis_log(
        const cis1::context_interface& ctx)
{
    global_logger.state.pid = ctx.pid();
    global_logger.state.ppid = ctx.ppid();

    auto cis_log = std::make_unique<file_logger>(
            ctx.base_dir() / "logs" / "cis.log",
              file_logger::log_parts::time
            | file_logger::log_parts::pid
            | file_logger::log_parts::ppid,
            std::ios_base::out | std::ios_base::app);

    if(!cis_log->is_open())
    {
        std::cerr << "Cant open cis.log file" << std::endl;
        exit(1);
    }

    global_logger.basic_loggers.cis_file = std::move(cis_log);
}

void init_session_log(
        const cis1::context_interface& ctx,
        const cis1::session_interface& session)
{
    global_logger.state.session_id = session.session_id();

    auto session_log = std::make_unique<file_logger>(
            ctx.base_dir() / "sessions"
            / (session.session_id() + ".log"),
              file_logger::log_parts::time
            | file_logger::log_parts::pid
            | file_logger::log_parts::ppid
            | file_logger::log_parts::session,
            std::ios_base::out | std::ios_base::app);

    if(!session_log->is_open())
    {
        std::cerr << "Cant open session log file" << std::endl;
        exit(1);
    }

    global_logger.basic_loggers.session_file = std::move(session_log);
}

std::ostream& cis_log()
{
    global_logger.combined_loggers.cis.prepare(global_logger.state);

    return *global_logger.cis_log_sink;
}

std::ostream& session_log()
{
    global_logger.combined_loggers.session.prepare(global_logger.state);

    return *global_logger.session_log_sink;
}

std::ostream& webui_log()
{
    global_logger.combined_loggers.remote.prepare(global_logger.state);

    return *global_logger.remote_log_sink;
}

std::ostream& tee_log()
{
    global_logger.combined_loggers.tee.prepare(global_logger.state);

    return *global_logger.tee_log_sink;
}
