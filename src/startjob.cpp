#include <iostream>

#include "context.h"
#include "session.h"
#include "build.h"
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

    std::cout << session.session_id() << std::endl;
    webui_session->auth(session);

    init_session_log(ctx, session);

    if(session.opened_by_me())
    {
        cis_log() << "action=\"open_session\"" << std::endl;
    }

    auto build_opt = cis1::prepare_build(ctx, job_name, ec, std_os);
    if(ec)
    {
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }
    auto& build = build_opt.value();

    if(!build.params().empty() && session.opened_by_me())
    {
        for(auto& [k, v] : build.params())
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
    else if(!build.params().empty())
    {
        build.prepare_params(ctx, session, ec);
    }

    if(ec)
    {
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }

    build.prepare_build_dir(session, ec);
    if(ec)
    {
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }

    cis1::set_value(ctx, session, "last_job_name", job_name, ec, std_os);
    if(ec)
    {
        tee_log() << ec.message() << std::endl;

        return 1;
    }

    ctx.set_env("job_name", job_name);

    cis1::set_value(ctx, session, "last_job_build_number", build.build_num(), ec, std_os);
    if(ec)
    {
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }

    ctx.set_env("build_number", build.build_num());

    session_log() << "action=\"start_job\" job_name=\"" << job_name << "\"" << std::endl;

    int exit_code = -1;

    build.execute(ctx, ec, exit_code);
    if(ec)
    {
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }

    session_log() << "action=\"finish_job\" job_name=\"" << job_name << "\"" << std::endl;

    std::cout << "session_id=" << session.session_id()
              << " action=start_job"
              << " job_name=" << job_name
              << " build_dir=" << build.build_num()
              << " pid=" << ctx.pid()
              << " ppid=" << ctx.ppid() << std::endl;
    std::cout << "Exit code: " << exit_code << std::endl;

    if(session.opened_by_me())
    {
        cis_log() << "action=\"close_session\"" << std::endl;
    }
}
