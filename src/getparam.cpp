#include "cis1_core.h"
#include <iostream>

void usage() {

  std::cout << "Usage:" << "\n";
  std::cout << "getparam param_name" << "\n";

  return;
}



int main( int argc, char *argv[] ) {

	cis1_core *cis;
	cis = new cis1_core;

	if( cis->init() != cis1_core::OK ) {
		std::cout << cis->getstatus_str() << std::endl;
    		exit(3);
	}


	if( argc !=2 ) {
		usage();
		// TODO cislog
		exit(1);
  	}


	if(cis->invoke_session() != 0) {
		std::cout << cis->getstatus_str() <<  std::endl;
		exit(3);
	}


	if (cis->session_opened_by_me() == true) {
		// TODO cis log, session log
		exit(1);
	}

	std::string param_value;

	if( cis->getparam( (std::string)argv[1],  &param_value ) != 0 ) {
		std::cout << cis->getstatus_str() << std::endl;
		exit(3);
	}

	std::cout << param_value << std::endl;

	delete cis;

	return 0;
}
