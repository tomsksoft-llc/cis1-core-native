/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include "logger.h"

#include <iostream>

#include <scl/recorder.h>
#include <scl/file_recorder.h>
#include <cis1_cwu_protocol/protocol.h>
#include <tpl_helpers/overloaded.h>

scl::LoggerPtr cis_logger;
scl::LoggerPtr session_logger;
scl::LoggerPtr webui_logger;

// session_id is a string in the %Y-%m-%d-%H-%M-%S-pid_ppid format, where:
// %Y-%m-%d-%H-%M-%S contains time_length_k
// - is 1 char
// pid contains pid_length_k
// _ is 1 char
// ppid contains pid_length_k
const std::size_t session_id_length
        = scl::detail::log_formatting::time_length_k + 2 * scl::detail::log_formatting::pid_length_k + 2;

class webui_recorder : public scl::IRecorder
{
public:
    explicit webui_recorder(std::shared_ptr<webui_session> session)
            : remote_endpoint_(std::move(session)),
              tr_(remote_endpoint_->make_transaction())
    {
    }

    ~webui_recorder() final = default;

    void OnRecord(const scl::RecordInfo& record) final
    {
        cis1::cwu::log_entry dto{};

        dto.message = to_webui_message(record.action, record.message);
        dto.time = std::chrono::system_clock::now();
        rtrim(dto.message);

        tr_.send(dto);
    }

private:
    static void rtrim(std::string& s)
    {
        s.erase(std::find_if(
                s.rbegin(),
                s.rend(),
                [](int ch)
                {
                    return !std::isspace(ch);
                }).base(),
                s.end());
    }

    static std::string to_webui_message(const std::optional<std::string> &action,
                                        const std::string &message) {
        if (!action) {
            return message;
        }

        std::stringstream result_stream;
        result_stream << R"(action=")" << *action << "\" " << message;
        return result_stream.str();
    }

    std::shared_ptr<webui_session> remote_endpoint_;
    cis1::proto_utils::transaction tr_;
};


scl::Logger::Options make_logger_options(
        const std::optional<std::string>& session_id,
        const cis1::context_interface& ctx,
        const cis1::os& std_os)
{
    scl::Logger::Options options;
    // TODO set the value from env var
    options.level = scl::Level::Debug;
    options.parent_pid = static_cast<scl::ProcessId>(ctx.ppid());
    options.pid = static_cast<scl::ProcessId>(ctx.pid());
    options.session_id = session_id;
    return options;
}

void cis_log(actions act, const std::string& message)
{
    if(!cis_logger)
    {
        return;
    }

    // TODO add a level param
    // now put the lowest level
    cis_logger->SesActRecord(scl::Level::Action, act, message);

    webui_log(act, message);
}

void session_log(actions act, const std::string& message)
{
    if(!session_logger)
    {
        return;
    }

    // TODO add a level param
    // now put the lowest level
    session_logger->ActRecord(scl::Level::Action, act, message);

    webui_log(act, message);
}

void webui_log(actions act, const std::string& message)
{
    if(webui_logger)
    {
        // TODO add a level param
        // now put the lowest level
        webui_logger->SesActRecord(scl::Level::Action, act, message);
    }
}

void tee_log(actions act, const std::string& message)
{
    cis_log(act, message);
    session_log(act, message);
    webui_log(act, message);
}

void init_webui_log(
        const scl::Logger::Options& options,
        const std::shared_ptr<webui_session>& session)
{
    auto recorder = std::make_unique<webui_recorder>(session);
    scl::RecordersCont recorders;
    recorders.push_back(std::move(recorder));

    webui_logger = std::visit(
            meta::overloaded{
                    [](scl::LoggerPtr&& val)
                    { return std::move(val); },
                    [](const auto&)
                    { /*return an empty unique_ptr*/ return scl::LoggerPtr{}; }
            },
            scl::Logger::Init(options, std::move(recorders))
    );
}

void init_cis_log(
        const scl::Logger::Options& options,
        const cis1::context_interface& ctx)
{
    const auto on_error
            = [](std::string_view error)
            {
                std::cerr << "Couldn't create a cis log: error = " << error << std::endl;
                exit(1);
            };

    scl::FileRecorder::Options recorder_options;
    recorder_options.log_directory = ctx.base_dir() / "logs";
    recorder_options.file_name_template = "cis.%n.log";

    // TODO set the recorder_options.size_limit
    recorder_options.align_info
            = scl::AlignInfo{
                max_action_name_length,
                session_id_length
            };
    auto recorder = std::visit(
            meta::overloaded{
                    [](scl::FileRecorderPtr&& val)
                    { return std::move(val); },
                    [&on_error](const auto& error)
                    {
                        on_error(scl::FileRecorder::ToStr(error));
                        return scl::FileRecorderPtr{};
                    }
            },
            scl::FileRecorder::Init(recorder_options)
    );

    scl::RecordersCont recorders;
    recorders.push_back(std::move(recorder));

    cis_logger = std::visit(
            meta::overloaded{
                    [](scl::LoggerPtr&& val)
                    { return std::move(val); },
                    [&on_error](const auto& error)
                    {
                        on_error(scl::Logger::ToStr(error));
                        return scl::LoggerPtr{};
                    },
            },
            scl::Logger::Init(options, std::move(recorders))
    );
}

void init_session_log(
        const scl::Logger::Options& options,
        const cis1::context_interface& ctx,
        const cis1::session_interface& session)
{
    const auto on_error
            = [](std::string_view error)
            {
                std::cerr << "Couldn't create a session log: error = " << error << std::endl;
                exit(1);
            };

    scl::FileRecorder::Options recorder_options;
    recorder_options.log_directory = ctx.base_dir() / "sessions";
    recorder_options.file_name_template = session.session_id() + ".%n.log";

    // TODO set the recorder_options.size_limit
    recorder_options.align_info = scl::AlignInfo{max_action_name_length};
    auto recorder = std::visit(
            meta::overloaded{
                    [](scl::FileRecorderPtr&& val)
                    { return std::move(val); },
                    [&on_error](const auto& error)
                    {
                        on_error(scl::FileRecorder::ToStr(error));
                        return scl::FileRecorderPtr{};
                    }
            },
            scl::FileRecorder::Init(recorder_options)
    );

    scl::RecordersCont recorders;
    recorders.push_back(std::move(recorder));

    session_logger = std::visit(
            meta::overloaded{
                    [](scl::LoggerPtr&& val)
                    { return std::move(val); },
                    [&on_error](const auto& error)
                    {
                        on_error(scl::Logger::ToStr(error));
                        return scl::LoggerPtr{};
                    }
            },
            scl::Logger::Init(options, std::move(recorders))
    );
}
