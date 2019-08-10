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
		std::cout << cis->getstatus_str() << "\n";
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


  char* tmp = getenv("session_id");
  if ( tmp !=nullptr )
	  std::cout << "session_id:" << (std::string)tmp << std::endl;
  else
	  std::cout << "Can't find session_id in env\n";



	if (cis->session_opened_by_me() == true) {
		// TODO set job params from console user input
	}

	int exit_code = 0;

	cis->startjob( argv[1],  &exit_code );

	delete cis;

	return 0;
}
