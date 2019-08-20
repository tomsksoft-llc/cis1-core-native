#include "cis1_core.h"

#include <iostream>

void usage()
{
    std::cout << "Usage:" << "\n"
              << "setparam param_name param_value" << std::endl;
}

int main(int argc, char *argv[])
{

    cis1_core cis;

    std::error_code ec;

    cis.init(ec);

    if(ec)
    {
        std::cout << ec.message() << std::endl;
        exit(3);
    }

    if(argc != 3)
    {
        usage();
        // TODO cislog
        exit(1);
      }

    if(cis.session_opened_by_me() == true)
    {
        // TODO cis log, session log
        exit(1);
    }

    cis.setparam(argv[1], argv[2], ec);
    if(ec)
    {
        // TODO cislog, session log
        std::cout << ec.message() << std::endl; // debug
        exit(3);
    }

    return 0;
}
