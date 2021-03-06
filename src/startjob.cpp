/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include <iostream>
#include <vector>
#include <string>
#include <optional>

#include <boost/program_options.hpp>
#include <cis1_proto_utils/param_codec.h>

#include "context.h"
#include "session.h"
#include "job.h"
#include "set_value.h"
#include "logger.h"
#include "os.h"
#include "webui_session.h"
#include "cis_version.h"

namespace po = boost::program_options;

std::optional<std::map<std::string, std::string>> prepared_params(po::variables_map& vm);

int main(int argc, char* argv[])
{
    po::options_description common_desc("Common options");
    common_desc.add_options()
        ("help", "produce help message")
        ("version", "print version");

    po::options_description startjob_desc("Startjob options");
    startjob_desc.add_options()
        ("force", "try to start job even if script is not executable")
        ("new_session", "makes new child session")
        ("params", po::value<std::vector<std::string>>()->multitoken(), "params passed to job");

    auto print_usage = [&]()
    {
        std::cout << "Usage: " << "\n"
                  << common_desc << "\n"
                  << startjob_desc << std::endl;
    };

    if(argc == 1)
    {
        print_usage();

        return EXIT_SUCCESS;
    }

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(2, argv, common_desc), vm);
    }
    catch(...)
    {
        std::cout << "Invalid args" << "\n";

        print_usage();

        return EXIT_FAILURE;
    }

    po::notify(vm);

    if(vm.count("help"))
    {
        print_usage();

        return EXIT_SUCCESS;
    }
    else if(vm.count("version"))
    {
        print_version();

        return EXIT_SUCCESS;
    }

    try
    {
        po::store(po::parse_command_line(argc - 1, argv + 1, startjob_desc), vm);
    }
    catch(...)
    {
        std::cout << "Invalid args" << "\n";

        print_usage();

        return EXIT_FAILURE;
    }

    po::notify(vm);

    bool force = vm.count("force");

    bool new_session = vm.count("new_session");

    auto predefined_params = prepared_params(vm);

    std::string job_name = argv[1];

    cis1::os std_os;

    std::error_code ec;

    auto ctx_opt = cis1::init_context(ec, std_os);
    if(ec)
    {
        std::cerr << ec.message() << std::endl;

        return 1;
    }
    auto& ctx = ctx_opt.value();

    auto session = cis1::invoke_session(ctx, new_session, std_os);

    const CoreLogger::Options options
            = make_logger_options(session.session_id(), ctx, std_os);

    auto webui_session = init_webui_session(ctx);

    if(webui_session != nullptr)
    {
        init_webui_log(options, webui_session);
    }

    init_cis_log(options, ctx);

    std::cout << session.session_id() << std::endl;

    if(webui_session)
    {
        webui_session->auth(session);
    }

    init_session_log(options, ctx, session);

    if(session.opened_by_me())
    {
        CIS_LOG(actions::open_session, "start");

        session.on_close(
                [](cis1::session_interface&)
                {
                    CIS_LOG(actions::close_session, "stop");
                });
    }

    if(session.opened_by_me())
    {
        WEBUI_LOG(actions::startjob_stdout, R"(%s)", session.session_id());
    }

    auto job_opt = cis1::load_job(job_name, ec, ctx, std_os);
    if(ec)
    {
        std::cout << ec.message() << std::endl;

        return EXIT_FAILURE;
    }
    auto& job = job_opt.value();

    auto params = job.params();
    if(!params.empty() && session.opened_by_me() && !predefined_params)
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
                std::string decoded_value;
                if(!cis1::proto_utils::decode_param(tmp, decoded_value))
                {
                    std::cout << "Invalid params" << "\n";
                    return EXIT_FAILURE;
                }

                v = decoded_value;
            }
        }
    }
    else if(predefined_params)
    {
        for(auto& [k, v] : params)
        {
            if(auto it = predefined_params->find(k);
                    it != predefined_params->end())
            {
                v = it->second;
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
        TEE_LOG(actions::error, "%s", ec.message());

        return 1;
    }

    if(auto var = std_os.get_env_var("job_name"); !var.empty())
    {
        ctx.set_env_var("parent_job_name", var);
    }

    if(auto var = std_os.get_env_var("build_number"); !var.empty())
    {
        ctx.set_env_var("parent_job_build_number", var);
    }

    ctx.set_env_var("job_name", job_name);

    ctx.set_env_var("build_number", build_handle.number_string());

    SES_LOG(actions::start_job, R"(job_name="%s")", job_name);

    int exit_code = -1;

    build_handle.execute(
            ctx,
            force,
            ec,
            [](bool error, const std::string& str)
            {
                WEBUI_LOG(error ? actions::startjob_stderr : actions::startjob_stdout, R"(%s)", str);
            },
            exit_code);
    if(ec)
    {
        std::cerr << ec.message() << std::endl;
        TEE_LOG(actions::error, "%s", ec.message());

        return 1;
    }

    SES_LOG(actions::finish_job, R"(job_name="%s")", job_name);

    if(!session.opened_by_me())
    {
        cis1::set_value(ctx, session, "last_job_name", job_name, ec, std_os);
        if(ec)
        {
            std::cerr << ec.message() << std::endl;
            TEE_LOG(actions::error, "%s", ec.message());

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
            TEE_LOG(actions::error, "%s", ec.message());

            return 1;
        }
    }

    std::stringstream ss;

    ss << "session_id=" << session.session_id()
       << " action=start_job"
       << " job_name=" << job_name
       << " build_dir=" << build_handle.number_string()
       << " pid=" << ctx.process_id()
       << " ppid=" << ctx.parent_startjob_id();

    if(session.opened_by_me())
    {
        WEBUI_LOG(actions::startjob_stdout, R"(%s)", ss.str());
    }

    std::cout << ss.str() << std::endl;

    ss.str("");

    ss << "Exit code: " << exit_code;

    if(session.opened_by_me())
    {
        WEBUI_LOG(actions::startjob_stdout, R"(%s)", ss.str());
    }

    std::cout << ss.str() << std::endl;

    std_os.spawn_process(
            ctx.base_dir(),
            std::filesystem::path{"core"} / ctx.get_env_var("maintenance"),
            {"--job", job_name},
            ctx.env());
}

std::optional<std::map<std::string, std::string>> prepared_params(po::variables_map& vm)
{
    if(vm.count("params"))
    {
        const auto& predefined_params_vec = vm["params"].as<std::vector<std::string>>();

        std::map<std::string, std::string> params;

        for(size_t i = 0; i < predefined_params_vec.size(); i += 2)
        {
            if(i + 1 < predefined_params_vec.size())
            {
                params[predefined_params_vec[i]] = predefined_params_vec[i + 1];
            }
            else
            {
                params[predefined_params_vec[i]] = {};
            }
        }

        return params;
    }

    return std::nullopt;
}
