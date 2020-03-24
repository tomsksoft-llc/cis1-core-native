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
#include <cis1_core_logger/core_record.h>
#include <cis1_cwu_protocol/protocol.h>
#include <tpl_helpers/overloaded.h>

using CoreRecord = cis1::core_logger::CoreRecord;
using RecordersCont = scl::RecordersCont<CoreRecord>;
using FileRecorder = scl::FileRecorder<CoreRecord>;
using FileRecorderPtr = scl::FileRecorderPtr<CoreRecord>;

LoggerPtr cis_logger;
LoggerPtr session_logger;
LoggerPtr webui_logger;
LoggerPtr offline_webui_logger;

class webui_recorder : public scl::IRecorder<CoreRecord>
{
public:
    explicit webui_recorder(std::shared_ptr<webui_session> session)
            : remote_endpoint_(std::move(session)),
              tr_(remote_endpoint_->make_transaction())
    {
    }

    ~webui_recorder() final = default;

    void OnRecord(const CoreRecord& record) final
    {
        cis1::cwu::log_entry dto{};

        dto.action = record.action ? record.action.value() : "";
        dto.message = record.message;
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

    std::shared_ptr<webui_session> remote_endpoint_;
    cis1::proto_utils::transaction tr_;
};


CoreLogger::Options make_logger_options(
        const std::optional<std::string>& session_id,
        const cis1::context_interface& ctx,
        const cis1::os& std_os)
{
    CoreLogger::Options options;
    // TODO set the value from env var
    options.level = scl::Level::Debug;
    options.parent_pid = static_cast<scl::ProcessId>(ctx.parent_startjob_id());
    options.pid = static_cast<scl::ProcessId>(ctx.process_id());
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

    if(offline_webui_logger)
    {
        // TODO add a level param
        // now put the lowest level
        offline_webui_logger->ActRecord(scl::Level::Action, act, message);
    }
}

void tee_log(actions act, const std::string& message)
{
    if(cis_logger)
    {
        // TODO add a level param
        // now put the lowest level
        cis_logger->SesActRecord(scl::Level::Action, act, message);
    }

    if(session_logger)
    {
        // TODO add a level param
        // now put the lowest level
        session_logger->ActRecord(scl::Level::Action, act, message);
    }

    webui_log(act, message);
}

void init_webui_log(
        const CoreLogger::Options& options,
        const std::shared_ptr<webui_session>& session)
{
    auto recorder = std::make_unique<webui_recorder>(session);
    RecordersCont recorders;
    recorders.push_back(std::move(recorder));

    webui_logger = std::visit(
            meta::overloaded{
                    [](LoggerPtr&& val)
                    { return std::move(val); },
                    [](const auto&)
                    { /*return an empty unique_ptr*/ return LoggerPtr{}; }
            },
            CoreLogger::Init(options, std::move(recorders))
    );
}

void init_cis_log(
        const CoreLogger::Options& options,
        const cis1::context_interface& ctx)
{
    const auto on_error
            = [](std::string_view error)
            {
                std::cerr << "Couldn't create a cis log: error = " << error << std::endl;
                exit(1);
            };

    // TODO set the recorder_options.size_limit
    FileRecorder::Options recorder_options;
    recorder_options.log_directory = ctx.base_dir() / "logs";
    recorder_options.file_name_template = "cis.%n.log";

    auto recorder = std::visit(
            meta::overloaded{
                    [](FileRecorderPtr&& val)
                    { return std::move(val); },
                    [&on_error](const auto& error)
                    {
                        on_error(FileRecorder::ToStr(error));
                        return FileRecorderPtr{};
                    }
            },
            FileRecorder::Init(recorder_options)
    );

    RecordersCont recorders;
    recorders.push_back(std::move(recorder));

    cis_logger = std::visit(
            meta::overloaded{
                    [](LoggerPtr&& val)
                    { return std::move(val); },
                    [&on_error](const auto& error)
                    {
                        on_error(CoreLogger::ToStr(error));
                        return LoggerPtr{};
                    },
            },
            CoreLogger::Init(options, std::move(recorders))
    );
}

void init_offline_webui_log(
        const CoreLogger::Options& options,
        const cis1::context_interface& ctx,
        const cis1::session_interface& session)
{
    const auto on_error
            = [](std::string_view error)
            {
                std::cerr << "Couldn't create a offline webui log: error = " << error << std::endl;
                exit(1);
            };

    // TODO set the recorder_options.size_limit
    FileRecorder::Options recorder_options;
    recorder_options.log_directory = ctx.base_dir() / "sessions";
    recorder_options.file_name_template = session.session_id() + ".combined.log";

    auto recorder = std::visit(
            meta::overloaded{
                    [](FileRecorderPtr&& val)
                    { return std::move(val); },
                    [&on_error](const auto& error)
                    {
                        on_error(FileRecorder::ToStr(error));
                        return FileRecorderPtr{};
                    }
            },
            FileRecorder::Init(recorder_options)
    );

    RecordersCont recorders;
    recorders.push_back(std::move(recorder));

    offline_webui_logger = std::visit(
            meta::overloaded{
                    [](LoggerPtr&& val)
                    { return std::move(val); },
                    [&on_error](const auto& error)
                    {
                        on_error(CoreLogger::ToStr(error));
                        return LoggerPtr{};
                    }
            },
            CoreLogger::Init(options, std::move(recorders))
    );
}

void init_session_log(
        const CoreLogger::Options& options,
        const cis1::context_interface& ctx,
        const cis1::session_interface& session)
{
    const auto on_error
            = [](std::string_view error)
            {
                std::cerr << "Couldn't create a session log: error = " << error << std::endl;
                exit(1);
            };

    // TODO set the recorder_options.size_limit
    FileRecorder::Options recorder_options;
    recorder_options.log_directory = ctx.base_dir() / "sessions";
    recorder_options.file_name_template = session.session_id() + ".%n.log";

    auto recorder = std::visit(
            meta::overloaded{
                    [](FileRecorderPtr&& val)
                    { return std::move(val); },
                    [&on_error](const auto& error)
                    {
                        on_error(FileRecorder::ToStr(error));
                        return FileRecorderPtr{};
                    }
            },
            FileRecorder::Init(recorder_options)
    );

    RecordersCont recorders;
    recorders.push_back(std::move(recorder));

    session_logger = std::visit(
            meta::overloaded{
                    [](LoggerPtr&& val)
                    { return std::move(val); },
                    [&on_error](const auto& error)
                    {
                        on_error(CoreLogger::ToStr(error));
                        return LoggerPtr{};
                    }
            },
            CoreLogger::Init(options, std::move(recorders))
    );

    init_offline_webui_log(options, ctx, session);
}
