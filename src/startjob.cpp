#include "cis1_core.h"
#include <iostream>

void usage() {

  std::cout << "Usage:" << "\n";
  std::cout << "startjob project/job" << "\n";

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
		// TODO set job params from console user input
	}

	int exit_code = 0;

	if( cis->startjob( argv[1],  &exit_code ) != 0 ) {
		std::cout << cis->getstatus_str() << std::endl;
		exit(3);
	}

	std::cout << "session_id=" << cis->get_session_id() << " action=start_job job_name=" << (std::string)(argv[1]) << std::endl;
	std::cout << "Exit code: " << std::to_string(exit_code) << std::endl;

	// TODO closesession to session log, corelog 
	delete cis;

	return exit_code;
}
