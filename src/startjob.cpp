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
#include "cis_version.h"

void usage()
{
    std::cout << "Usage:" << "\n"
              << "startjob project/job [--force]" << "\n";
}

int main(int argc, char* argv[])
{
    if(argc == 2 && strcmp(argv[1], "--version") == 0)
    {
        print_version();

        return EXIT_SUCCESS;
    }

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

    if(argc > 3 || argc < 2)
    {
        usage();

        return 1;
    }

    if(argc == 3 && strcmp(argv[2], "--force") != 0)
    {
        usage();

        return 1;
    }

    bool force = (argc == 3);

    std::string job_name = argv[1];

    auto session_opt = cis1::invoke_session(ctx, ec, std_os);
    if(ec)
    {
        std::cerr << ec.message() << std::endl;
        cis_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }
    auto& session = session_opt.value();

    std::cout << session.session_id() << std::endl;
    if(webui_session)
    {
        webui_session->auth(session);
    }

    init_session_log(ctx, session);

    if(session.opened_by_me())
    {
        cis_log() << "action=\"open_session\"" << std::endl;

        session.on_close(
                [](cis1::session_interface&)
                {
                    cis_log() << "action=\"close_session\"" << std::endl;
                });
    }

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
        std::cerr << ec.message() << std::endl;
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }

    if(auto var = std_os.get_env_var("job_name"); !var.empty())
    {
        ctx.set_env("parent_job_name", var);
    }

    if(auto var = std_os.get_env_var("build_number"); !var.empty())
    {
        ctx.set_env("parent_job_build_number", var);
    }

    ctx.set_env("job_name", job_name);

    ctx.set_env("build_number", build_handle.number_string());

    session_log() << "action=\"start_job\" job_name=\""
                  << job_name << "\"" << std::endl;

    int exit_code = -1;

    build_handle.execute(ctx, force, ec, exit_code);
    if(ec)
    {
        std::cerr << ec.message() << std::endl;
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }

    session_log() << "action=\"finish_job\" job_name=\""
                  << job_name << "\"" << std::endl;

    if(!session.opened_by_me())
    {
        cis1::set_value(ctx, session, "last_job_name", job_name, ec, std_os);
        if(ec)
        {
            std::cerr << ec.message() << std::endl;
            tee_log() << "action=\"error\" " << ec.message() << std::endl;

            return 1;
        }

        cis1::set_value(
                ctx,
                session,
                "last_job_build_number",
                build_handle.number_string(),
                ec,
                std_os);
        if(ec)
        {
            std::cerr << ec.message() << std::endl;
            tee_log() << "action=\"error\" " << ec.message() << std::endl;

            return 1;
        }
    }

    std::cout << "session_id=" << session.session_id()
              << " action=start_job"
              << " job_name=" << job_name
              << " build_dir=" << build_handle.number_string()
              << " pid=" << ctx.pid()
              << " ppid=" << ctx.ppid() << std::endl;

    std::cout << "Exit code: " << exit_code << std::endl;

    std_os.spawn_process(
            ctx.base_dir(),
            std::filesystem::path{"core"} / ctx.env().at("maintenance").to_vector()[0],
            {"--job", job_name},
            ctx.env());
}
