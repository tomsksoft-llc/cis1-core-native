/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include <iostream>

#include "context.h"
#include "session.h"
#include "job.h"
#include "set_value.h"
#include "logger.h"
#include "os.h"
#include "webui_session.h"

void usage()
{
    std::cout << "Usage:" << "\n"
              << "startjob project/job" << "\n";
}

int main(int argc, char* argv[])
{
    cis1::os std_os;

    std::error_code ec;

    auto ctx_opt = cis1::init_context(ec, std_os);
    if(ec)
    {
        std::cerr << ec.message() << std::endl;

        return 1;
    }
    auto& ctx = ctx_opt.value();

    auto webui_session = init_webui_session(ctx);

    if(webui_session != nullptr)
    {
        init_webui_log(webui_session);
    }

    init_cis_log(ctx);

    if(argc != 2)
    {
        usage();

        return 1;
    }
    std::string job_name = argv[1];

    auto session_opt = cis1::invoke_session(ctx, ec, std_os);
    if(ec)
    {
        cis_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }
    auto& session = session_opt.value();

    if(webui_session)
    {
        webui_session->auth(session);
    }

    init_session_log(ctx, session);

    if(session.opened_by_me())
    {
        cis_log() << "action=\"open_session\"" << std::endl;
    }

    if(session.opened_by_me())
    {
        webui_log() << "action=\"root_job_stdout\" "
                    << session.session_id() << std::endl;
    }

    std::cout << session.session_id() << std::endl;

    auto job_opt = cis1::load_job(job_name, ec, ctx, std_os);
    if(ec)
    {
        std::cout << ec.message() << std::endl;

        return EXIT_FAILURE;
    }
    auto& job = job_opt.value();

    auto params = job.params();
    if(!params.empty() && session.opened_by_me())
    {
        for(auto& [k, v] : params)
        {
            std::cout << "Type param value for parameter " << k
                      << "(\"Enter\" for default value:\""
                      << v << "\")" << std::endl;

            std::string tmp;
            std::getline(std::cin, tmp, '\n');

            if(!tmp.empty())
            {
                v = tmp;
            }
        }
    }
    else if(!params.empty())
    {
        cis1::prepare_params(params, std_os, ctx, session, ec);
    }

    auto build_handle = job.prepare_build(
            ctx,
            session,
            params,
            ec);
    if(ec)
    {
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }

    ctx.set_env_var("parent_startjob_id", std::to_string(ctx.process_id()));

    cis1::set_value(ctx, session, "last_job_name", job_name, ec, std_os);
    if(ec)
    {
        tee_log() << ec.message() << std::endl;

        return 1;
    }

    ctx.set_env_var("job_name", job_name);

    cis1::set_value(
            ctx,
            session,
            "last_job_build_number",
            build_handle.number_string(),
            ec,
            std_os);
    if(ec)
    {
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }

    ctx.set_env_var("build_number", build_handle.number_string());

    session_log() << "action=\"start_job\" job_name=\""
                  << job_name << "\"" << std::endl;

    int exit_code = -1;

    build_handle.execute(
            ctx,
            ec,
            [](bool error, const std::string& str)
            {
                webui_log() << "action=\""
                            << (error ? "stderr" : "stdout")
                            << "\" "
                            << str << std::endl;
            },
            exit_code);
    if(ec)
    {
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }

    session_log() << "action=\"finish_job\" job_name=\""
                  << job_name << "\"" << std::endl;

    std::stringstream ss;

    ss << "session_id=" << session.session_id()
              << " job_name=" << job_name
              << " build_dir=" << build_handle.number_string()
              << " pid=" << ctx.process_id()
              << " ppid=" << ctx.parent_startjob_id();

    if(session.opened_by_me())
    {
        webui_log() << "action=\"root_job_stdout\" " << ss.str() << std::endl;
    }

    std::cout << ss.str() << std::endl;

    ss.str("");

    ss << "Exit code: " << exit_code;

    if(session.opened_by_me())
    {
        webui_log() << "action=\"root_job_stdout\" " << ss.str() << std::endl;
    }

    std::cout << ss.str() << std::endl;

    if(session.opened_by_me())
    {
        cis_log() << "action=\"close_session\"" << std::endl;
    }

    std_os.spawn_process(
            ctx.base_dir(),
            std::filesystem::path{"core"} / ctx.get_env_var("maintenance"),
            {"--job", job_name},
            ctx.env());
}
