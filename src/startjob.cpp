#include "cis1_core.h"
#include <iostream>

void usage()
{
    std::cout << "Usage:" << "\n"
              << "startjob project/job" << "\n";
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

    if(argc != 2)
    {
        usage();
        // TODO cislog
        exit(1);
      }

    int exit_code = 0;

    cis.startjob(argv[1], exit_code, ec);
    if(ec)
    {
        std::cout << ec.message() << std::endl;
        exit(3);
    }

    std::cout << "session_id=" << cis.get_session_id()
              << " action=start_job job_name="
              << argv[1] << std::endl;

    std::cout << "Exit code: " << exit_code << std::endl;

    // TODO closesession to session log, corelog

    return exit_code;
}
