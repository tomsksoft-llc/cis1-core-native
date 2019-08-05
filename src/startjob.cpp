#include "cis1_core.h"
#include <iostream>

void usage() {

  std::cout << "Usage:" << "\n";
  std::cout << "startjob project/job" << "\n";

  return;

}



int main( int argc, char *argv[] ) {

  if( argc !=2 ) {
    usage();
    exit(1);
  }

  cis1_core *cis;
  cis = new cis1_core;

  if( cis->init() != cis1_core::OK ) {

    std::cout << cis->getstatus_str() << "\n";
    exit(3);

  }


  cis->startjob( argv[1] );

  delete cis;

  return 0;
}
