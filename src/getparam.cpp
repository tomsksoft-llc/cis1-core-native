#include "cis1_core.h"

#include <iostream>

void usage()
{
    std::cout << "Usage:" << "\n"
              << "getparam param_name" << std::endl;
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

	if(cis.session_opened_by_me() == true)
    {
		// TODO cis log, session log
		exit(1);
	}

	std::string param_value;

	cis.getparam(argv[1], param_value, ec);
    if(ec)
    {
		std::cout << ec.message() << std::endl;
		exit(3);
	}

	std::cout << param_value << std::endl;

	return 0;
}
