#include <iostream>

#include "cis1_context.h"
#include "session.h"
#include "set_param.h"
#include "logger.h"

void usage()
{
    std::cout << "Usage:" << "\n"
              << "setparam param_name param_value" << std::endl;
}

int main(int argc, char *argv[])
{
    os std_os;

    std::error_code ec;

    if(argc != 3)
    {
        //...
        return 1;
    }

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

    if(session.opened_by_me())
    {
        //...
        return 1;
    }

    set_param(ctx, session, argv[1], argv[2], ec);
    if(ec)
    {
        //...
        return 1;
    }

    return 0;
}
