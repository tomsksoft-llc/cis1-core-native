#include "cis1_core.h"

#include <iostream>

void usage() {

  std::cout << "Usage:" << "\n";
  std::cout << "setparam param_name param_value" << "\n";

  return;
}



int main( int argc, char *argv[] ) {

	cis1_core cis;

	if( cis.init() != cis1_core::TStatus::OK ) {
		std::cout << cis.getstatus_str() << std::endl;
    	exit(3);
	}


	if( argc != 3 ) {
		usage();
		// TODO cislog
		exit(1);
  	}


	if( cis.invoke_session() != 0 ) {
		// TODO cislog
		std::cout << cis.getstatus_str() << std::endl; // debug
		exit(3);
	}


	if ( cis.session_opened_by_me() == true ) {
		// TODO cis log, session log
		exit(1);
	}


	if( cis.setparam( argv[1], argv[2] ) != 0 ) {
		// TODO cislog, session log
		std::cout << cis.getstatus_str() << std::endl; // debug
		exit(3);
	}

	return 0;
}
