#include <iostream>

#include "context.h"
#include "session.h"
#include "get_value.h"
#include "logger.h"
#include "os.h"
#include "webui_session.h"

void usage()
{
    std::cout << "Usage:" << "\n"
              << "getvalue value_name" << std::endl;
}

int main(int argc, char *argv[])
{

    cis1::os std_os;

    std::error_code ec;

    auto ctx_opt = cis1::init_context(ec, std_os);
    if(ec)
    {
        std::cerr << ec.message() << std::endl;;

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

    init_session_log(ctx, session);

    if(argc != 2)
    {
        tee_log()  << "action=\"error\" "
                    << "Wrong args count in getvalue" << std::endl;

        return 1;
    }

    if(session.opened_by_me())
    {
        tee_log()  << "action=\"error\" "
                    << "Cant get value outside the session" << std::endl;

        return 1;
    }

    auto value_opt = cis1::get_value(ctx, session, argv[1], ec, std_os);
    if(ec)
    {
        tee_log() << "action=\"error\" " << ec.message() << std::endl;

        return 1;
    }

    std::cout << value_opt.value() << std::endl;

    return 0;
}
