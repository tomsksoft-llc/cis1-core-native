#include <iostream>

#include "context.h"
#include "session.h"
#include "build.h"
#include "set_value.h"
#include "logger.h"
#include "os.h"

void usage()
{
    std::cout << "Usage:" << "\n"
              << "startjob project/job" << "\n";
}

std::optional<std::string> check_startjob_args(
        int argc,
        char* argv[],
        std::error_code& ec)
{
    if(argc != 2)
    {
        ec.assign(1, ec.category()); // FIXME
        return std::nullopt;
    }

    return argv[1];
}

int main(int argc, char* argv[])
{
    cis1::os std_os;

    std::error_code ec;

    auto ctx_opt = cis1::init_context(ec, std_os);
    if(ec)
    {
        //...
        return 1;
    }
    auto& ctx = ctx_opt.value();

    auto session_opt = cis1::invoke_session(ctx, ec, std_os);
    if(ec)
    {
        //...
        return 1;
    }
    auto& session = session_opt.value();

    auto job_name_opt = check_startjob_args(argc, argv, ec);
    if(ec)
    {
        usage();
        return 1;
    }
    auto& job_name = job_name_opt.value();

    auto build_opt = cis1::prepare_build(ctx, job_name, ec, std_os);
    if(ec)
    {
        //...
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
        //...
        return 1;
    }

    build.prepare_build_dir(ec);
    if(ec)
    {
        //...
        return 1;
    }

    cis1::set_value(ctx, session, "last_job_name", job_name, ec, std_os);
    if(ec)
    {
        //...
        return 1;
    }

    ctx.set_env("job_name", job_name);

    cis1::set_value(ctx, session, "last_job_build_number", build.build_num(), ec, std_os);
    if(ec)
    {
        //...
        return 1;
    }

    ctx.set_env("build_number", build.build_num());

    /*
    LOG(ctx, session) << "action=\"start_job\"" << std::endl;
    */

    int exit_code = -1;

    build.execute(ctx, ec, exit_code);
    if(ec)
    {
        //...
        return 1;
    }

    /*
    LOG(ctx, session) << "action=\"start_job\" exit_code="
                      << exit_code << std::endl;
    */

    std::cout << session.session_id() << std::endl;
    std::cout << "session_id=" << session.session_id()
              << " action=start_job"
              << " job_name=" << job_name
              << " build_dir=" << build.build_num()
              << " pid=" << boost::this_process::get_id()
              << " ppid=" << 0 /*FIXME*/ << std::endl;
    std::cout << "Exit code: " << exit_code << std::endl;
}
