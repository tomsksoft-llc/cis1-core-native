/*! \file cis1_core.cpp
	\brief Class cis1_core implementation.

Class cis1_core implementation.

*/

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

/*! \class cis1_core
	\brief The cis1_core class incapsulate all cis1 core functionality.

	The Init must be called before using any other function.
	To test status call getstatus and then getstatus_str.

*/

class cis1_core {

public:


	/*! \enum cis1_core object statuses
		\brief cis1_core object statuses
	*/

	enum TStatus {

		OK,
		ERROR_NOT_INIT,
		ERROR_ALREADY_INIT,
		ERROR_CIS_BASE_DIR_ENV_NOT_DEFINED,
		ERROR_CIS_BASE_DIR_NOTEXIST,
		ERROR_CAN_NOT_OPEN_SESSION,
		ERROR_CANT_EVAL_NEW_BUILD_NUM,
		ERROR_CANT_CREATE_BUILD_DIR,
		ERROR_CANT_COPY_SCRIPT_TO_BUILD_DIR,
		ERROR_CANT_EXECUTE_SCRIPT
	};

	cis1_core();
	~cis1_core();

	TStatus init();

	TStatus getstatus();
	std::string getstatus_str();

	bool session_opened_by_me() {
		return session_opened_by_me_flag;
	};

	std::string get_session_id() {
		return session_id;
	}

	int startjob(std::string jobname);
	int setparam(std::string param_name, std::string param_value);
	std::string getparam(std::string param_name);

	int setvalue(std::string param_name, std::string param_value);
	std::string getvalue(std::string param_name);

private:

	std::string cis_base_dir;
	TStatus status;
	bool session_opened_by_me_flag;
	std::string session_id;

	int invoke_session();

	std::string get_new_build_dir(std::string jobname);
	int create_dir(std::string dirname);
	int copy_file(std::string src, std::string dst);
	int execute_script_in_dir(std::string dir, std::string script_name, std::string log_file_name, std::string exit_code_file_name, int* exit_code);
};


cis1_core::cis1_core() {

	status = cis1_core::ERROR_NOT_INIT;
	cis_base_dir = "";
	session_id = "";
	session_opened_by_me_flag = false;

}


cis1_core::~cis1_core() {

	// destructor

}

/*! \fn init
	\brief Initialize internal state for next works
	@return cis1_core::TStatus initialization result
*/
cis1_core::TStatus cis1_core::init() {

	if (status != cis1_core::ERROR_NOT_INIT) {
		return cis1_core::ERROR_ALREADY_INIT;
	}

	char* tmp = getenv("cis_base_dir");

	if (!tmp) {
		status = cis1_core::ERROR_CIS_BASE_DIR_ENV_NOT_DEFINED;
		return status;
	}

	cis_base_dir = tmp;


	struct stat info;

	if (stat(cis_base_dir.c_str(), &info) != 0) {
		status = cis1_core::ERROR_CIS_BASE_DIR_NOTEXIST;
		return status;
	}

	if (!(info.st_mode & S_IFDIR)) {
		status = cis1_core::ERROR_CIS_BASE_DIR_NOTEXIST;
		return status;
	}

	// TODO: init corelog system

	if (invoke_session() != 0) {
		status = cis1_core::ERROR_CAN_NOT_OPEN_SESSION;
		// TODO: corelog
		return status;
	}

	// TODO: corelog
	// TODO: init sessionlog, log about new sessoin if need
	status = cis1_core::OK;
	return status;

}


/*! \fn getstatus
	\brief Return current status of the object
	@return TStatus Current status
*/

cis1_core::TStatus cis1_core::getstatus() {

	return status;

}

/*! \fn getstatus_str
	\brief return string description of the current status
	@return string Current status description
*/

std::string cis1_core::getstatus_str() {

	switch (status) {

	case cis1_core::OK: return (std::string)"OK";
	case cis1_core::ERROR_NOT_INIT: return (std::string)"Object not initialized";
	case cis1_core::ERROR_CIS_BASE_DIR_ENV_NOT_DEFINED: return (std::string)"The cis_base_dir not defined in environment";
	default: return (std::string)"Undefined error";

	}

}


/*! \fn startjob
	\brief starting job
	@return int 0 if job was successfully started, non zero if any error
*/
int cis1_core::startjob(std::string jobname, int* exit_code) {

	if (status != cis1_core::ERROR_NOT_INIT) {
		return cis1_core::ERROR_ALREADY_INIT;
	}

	// TODO: check jobname env consistent (dir, config file, script)
	std::string script_name = "script"; // TODO: read script name form job config file

	std::string build_num = get_new_build_dir(jobname); // TODO: decompose logic

	if (build_num == nullptr) {
		status = cis1_core::ERROR_CANT_EVAL_NEW_BUILD_NUM;
		// TODO: corelog, session log
		return 1;
	}

	if (create_dir(cis_base_dir + jobname + build_num) != 0) {
		status = cis1_core::ERROR_CANT_CREATE_BUILD_DIR;
		// TODO corelog, session log
		return 1;
	}

	if (copy_file(cis_base_dir + jobname + script_name, cis_base_dir + jobname + build_num + script_name) != 0) {
		status = cis1_core::ERROR_CANT_COPY_SCRIPT_TO_BUILD_DIR;
		// TODO corelog, session log
		return 1;
	}


	if (execute_script_in_dir(cis_base_dir + jobname + build_num, script_name, "log.txt", "exitcode.txt", exit_code) != 0) {
		status = cis1_core::ERROR_CANT_EXECUTE_SCRIPT;
		// TODO corelog, session log
		return 1;
	}

	// TODO: corelog, session log
	return 0;
}

int cis1_core::setparam(std::string param_name, std::string param_value) { return 0; }

/*
  Connect to exist session or create new if need
*/
int cis1_core::invoke_session() {
	char* tmp = getenv("session_id");

	if (tmp != nullptr) {
		session_id = (std::string)tmp;
		session_opened_by_me_flag = false;
		return 0;
	}

	std::string _sid = "2019-08-07-00-00-541";
	if (setenv("session_id", _sid.c_str(), 1) != 0) {
		status = cis1_core::ERROR_CAN_NOT_OPEN_SESSION;
		return 1;
	}

	session_id = _sid;
	session_opened_by_me_flag = true;
	return 0;

}

std::string get_new_build_dir(std::string jobname) { return "000000"; }
int create_dir(std::string dirname) { return 0 };
int copy_file(std::string src, std::string dst) {return 0 };
int execute_script_in_dir(std::string dir, std::string script_name, std::string log_file_name, std::string exit_code_file_name, int* exit_code) { return 0 };
