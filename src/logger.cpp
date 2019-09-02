#include "logger.h"

#include <fstream>
#include <memory>
#include <iomanip>
#include <iostream>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/device/null.hpp>

struct logger
{
    size_t pid;
    size_t ppid;

    std::optional<std::string> session_id;

    std::unique_ptr<std::ostream> default_sink = 
            std::make_unique<
                    boost::iostreams::stream<boost::iostreams::null_sink>>();

    std::unique_ptr<std::ofstream> cis_log_sink = nullptr;

    std::unique_ptr<std::ofstream> session_log_sink = nullptr;

    std::unique_ptr<
            boost::iostreams::tee_device<
                    std::ostream,
                    std::ostream>> both_log_device;
    
    std::unique_ptr<std::ostream> both_log_sink = nullptr;

    void write_time(std::ostream& os)
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto id = boost::this_process::get_id();
        os << std::put_time(std::localtime(&time), "%Y-%m-%d-%H-%M-%S");
    }

    void write_pid(std::ostream& os)
    {
        os << std::setw(8) << pid;
    }

    void write_ppid(std::ostream& os)
    {
        os << std::setw(8) << ppid;
    }

    void write_session(std::ostream& os)
    {
        os << std::setw(38);

        if(!session_id)
        {
            os << "unknown";
        }
        else
        {
            os << *session_id;
        }
    }

};

logger state;

void init_cis_log(
        const cis1::context_interface& ctx)
{
    state.cis_log_sink = std::make_unique<std::ofstream>(
            ctx.base_dir() / "logs" / "cis.log",
            std::ios_base::out | std::ios_base::app);

    state.pid = ctx.pid();
    state.ppid = ctx.ppid();

    if(!state.cis_log_sink->is_open())
    {
        std::cerr << "Cant open cis.log file" << std::endl;
        exit(1);
    }
}

void init_session_log(
        const cis1::context_interface& ctx,
        const cis1::session_interface& session)
{
    state.session_log_sink = std::make_unique<std::ofstream>(
            ctx.base_dir() / "sessions"
            / (session.session_id() + ".log"),
            std::ios_base::out | std::ios_base::app);

    state.session_id = session.session_id();

    if(!state.session_log_sink->is_open())
    {
        std::cerr << "Cant open session log file" << std::endl;
        exit(1);
    }

    if(state.cis_log_sink)
    {
        state.both_log_device = std::make_unique<
                boost::iostreams::tee_device<
                        std::ostream,
                        std::ostream>>(
                        *state.cis_log_sink,
                        *state.session_log_sink);

        state.both_log_sink = std::make_unique<
                boost::iostreams::stream<
                        boost::iostreams::tee_device<
                                std::ostream,
                                std::ostream>>>(*state.both_log_device);
    }
}

std::ostream& cis_log()
{
    if(state.cis_log_sink)
    {
        state.write_time(*state.cis_log_sink);

        *state.cis_log_sink << " | ";

        state.write_pid(*state.cis_log_sink);

        *state.cis_log_sink << " | ";

        state.write_ppid(*state.cis_log_sink);

        *state.cis_log_sink << " | ";

        state.write_session(*state.cis_log_sink);

        *state.cis_log_sink << " | ";

        return *state.cis_log_sink;
    }

    return *state.default_sink;
}

std::ostream& session_log()
{
    if(state.session_log_sink)
    {
        state.write_time(*state.session_log_sink);

        *state.session_log_sink << " | ";

        state.write_pid(*state.session_log_sink);

        *state.session_log_sink << " | ";

        state.write_ppid(*state.session_log_sink);

        *state.session_log_sink << " | ";

        return *state.session_log_sink;
    }

    return *state.default_sink;
}

std::ostream& both_log()
{
    if(state.both_log_sink)
    {
        cis_log();
        session_log();

        return *state.both_log_sink;
    }

    return *state.default_sink;
}
