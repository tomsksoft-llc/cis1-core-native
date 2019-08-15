#include "cis1_core.h"

#include <iostream>

void usage()
{
    std::cout << "Usage:" << "\n"
              << "getvalue value_name" << std::endl;
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

	std::string value;

	cis.getvalue(argv[1], value, ec);
    if(ec)
    {
		std::cout << ec.message() << std::endl;
		exit(3);
	}

	std::cout << value << std::endl;

	return 0;
}
