/* OS Depended functions */

#include <cstdlib>
#include <iostream>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



std::string cis1_core::get_env_var( std::string var_name ) {

	char *buf= std::getenv(var_name.c_str());
	std::string ret = "";

	if( buf != nullptr ) {
		ret.assign(buf);
	}

	return ret;
}



int cis1_core::set_env_var( std::string var_name, std::string value ) {

	return setenv(var_name.c_str(), value.c_str(), 1);
}


int cis1_core::is_dir( std::string dir ) {

        struct stat info;

        if (stat(dir.c_str(), &info) != 0)
                return 1;

        if (!(info.st_mode & S_IFDIR))
                return 1;

	return 0;
}

std::string cis1_core::get_new_build_dir(std::string dir) { return "000000"; }
int cis1_core::create_dir(std::string dir) { return 0; }
int cis1_core::copy_file(std::string src, std::string dst) { return 0; }


int cis1_core::execute_script_in_dir(
        std::string dir,
        std::string script_file_name,
        std::string log_file_name,
        std::string exit_code_file_name,
        int* exit_code)
{

        std::string command = "./"+script_file_name+" >"+log_file_name;

        //std::cout << dir << " " << command << std::endl;

        chdir(dir.c_str());
        *exit_code = std::system( command.c_str());
        return 0;
}

int cis1_core::read_file_str(std::string fname, std::vector<std::string> *lines) {

        lines->push_back("s1=s1 v");
        lines->push_back("s2=s2v");
        return 0;

}

