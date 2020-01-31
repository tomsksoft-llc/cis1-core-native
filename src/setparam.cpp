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
#include "set_param.h"
#include "logger.h"
#include "os.h"
#include "webui_session.h"
#include "cis_version.h"

void usage()
{
    std::cout << "Usage:" << "\n"
              << "setparam param_name param_value" << std::endl;
}

int main(int argc, char *argv[])
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

    auto session = cis1::invoke_session(ctx, false, std_os);

    const scl::Logger::Options options
            = make_logger_options(session.session_id(), ctx, std_os);

    auto webui_session = init_webui_session(ctx);

    if(webui_session != nullptr)
    {
        init_webui_log(options, webui_session);
    }
    init_cis_log(options, ctx);

    if(webui_session)
    {
        webui_session->auth(session);
    }

    init_session_log(options, ctx, session);

    if(argc != 3)
    {
        std::cerr << "Wrong arguments" << std::endl;
        TEE_LOG(actions::error, "Wrong args count in setparam");

        return 1;
    }

    if(session.opened_by_me())
    {
        std::cerr << "Cant start setparam outside of session" << std::endl;
        TEE_LOG(actions::error, "Cant set param outside the session");

        return 1;
    }

    cis1::set_param(ctx, session, argv[1], argv[2], ec, std_os);
    if(ec)
    {
        std::cerr << ec.message() << std::endl;
        TEE_LOG(actions::error, "%s", ec.message());

        return 1;
    }

    SES_LOG(actions::setparam, R"("%s"="%s")", argv[1], argv[2]);
    return 0;
}
