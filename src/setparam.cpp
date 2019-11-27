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

void usage()
{
    std::cout << "Usage:" << "\n"
              << "setparam param_name param_value" << std::endl;
}

int main(int argc, char *argv[])
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

    if(session.opened_by_me())
    {
        tee_log()  << "action=\"error\" "
                    << "Cant set param outside the session" << std::endl;

        return 1;
    }

    init_session_log(ctx, session);

    if(argc != 3)
    {
        tee_log()  << "action=\"error\" "
                    << "Wrong args count in setparam" << std::endl;

        return 1;
    }

    cis1::set_param(ctx, session, argv[1], argv[2], ec, std_os);
    if(ec)
    {
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }

    session_log()   << "action=\"setparam\" \""
                    << argv[1] << "\"=\"" << argv[2] << "\"" << std::endl;

    return 0;
}
