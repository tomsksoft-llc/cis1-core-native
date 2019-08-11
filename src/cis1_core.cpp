/*

	Class cis1_core members implementation

*/

#include "cis1_core.h"
#include "cis1_core_osdp.cpp"


cis1_core::TStatus cis1_core::init() {

	if (status != cis1_core::NOT_INIT) {
		return cis1_core::ERROR_ALREADY_INIT;
	}

	cis_base_dir = get_env_var("cis_base_dir");

	if (cis_base_dir.empty()) {
		status = cis1_core::ERROR_CIS_BASE_DIR_ENV_NOT_DEFINED;
		return status;
	}

	if (is_dir(cis_base_dir.c_str()) != 0) {
		status = cis1_core::ERROR_CIS_BASE_DIR_NOTEXIST;
		return status;
	}

	// TODO: init corelog system

	if (invoke_session() != 0) {
		status = cis1_core::ERROR_CANT_OPEN_SESSION;
		// TODO: corelog
		return status;
	}

	// TODO: corelog
	// TODO: init sessionlog, log about new sessoin if need
	status = cis1_core::OK;
	return status;

}



/*! \fn invoke_session
	\brief   Connect to exist session or create new if need
	@return int 0 if success, non zero otherwise
*/
int cis1_core::invoke_session() {

	//TODO add check object status

	session_id = get_env_var("session_id");

	if (!session_id.empty()) {
		session_opened_by_me_flag = false;
		return 0;
	}

	// TODO write properly session_id compute
	session_id = "2019-08-07-00-00-541";
	if (set_env_var("session_id", session_id.c_str()) != 0) {
		status = cis1_core::ERROR_CANT_OPEN_SESSION;
		return 1;
	}

	session_opened_by_me_flag = true;
	return 0;

}



/*! \fn getstatus_str
	\brief return string description of the current status
	@return string Current status description
*/

std::string cis1_core::getstatus_str() {

	switch (status) {

	case cis1_core::OK: return (std::string)"OK";
	case cis1_core::NOT_INIT: return (std::string)"Object not initialized";
	case cis1_core::ERROR_CIS_BASE_DIR_ENV_NOT_DEFINED: return (std::string)"The cis_base_dir not defined in environment";
	default: return (std::string)"Undefined error:"+std::to_string(status);

	}

}


/*! \fn startjob
	\brief starting job
	@return int 0 if job was successfully started, non zero if any error
*/
int cis1_core::startjob(std::string job_name, int* exit_code) {

	if (status != cis1_core::OK) {
		return 1;
	}


	std::string job_dir = cis_base_dir+"/jobs/"+job_name;

	// TODO: check jobname env consistent (dir, config file, script)
	std::string script_file_name = "script"; // TODO: read script name form job config file

	std::string build_num = get_new_build_dir(job_dir);

	if (build_num.empty()) {
		status = cis1_core::ERROR_CANT_EVAL_NEW_BUILD_NUM;
		// TODO: corelog, session log
		return 1;
	}

	if (create_dir(job_dir+"/"+build_num) != 0) {
		status = cis1_core::ERROR_CANT_CREATE_BUILD_DIR;
		// TODO corelog, session log
		return 1;
	}

	if (copy_file(job_dir+"/"+script_file_name, job_dir+"/"+build_num+"/"+script_file_name) != 0) {
		status = cis1_core::ERROR_CANT_COPY_SCRIPT_TO_BUILD_DIR;
		// TODO corelog, session log
		return 1;
	}

	if (execute_script_in_dir(job_dir+"/"+build_num, script_file_name, "log.txt", "exitcode.txt", exit_code) != 0) {
		status = cis1_core::ERROR_CANT_EXECUTE_SCRIPT;
		// TODO corelog, session log
		return 1;
	}

	// TODO: corelog, session log
	return 0;
}

int cis1_core::setparam(std::string param_name, std::string param_value) { 

	// TODO check status, check session

	// TODO sheck if param exist in job.param

	// TODO read session prm file and replace the param value or add it to the prm file

	return 0;

}

int cis1_core::getparam(std::string param_name, std::string *param_value) {

	param_value->assign("s3v");

	// TODO check status, check session

	// TODO check if param exist in job.param

	// TODO read session prm file and find and return valie  
	// ERRROR if param not found in session prm file

	return 0;

}


int cis1_core::get_job_params_list( std::string job_name, std::vector<std::string> *params_list ) {

	// TODO check object status


	// TODO check if job.params file exists, if no return 0

	if( read_file_str(cis_base_dir+"/jobs/"+job_name+"/job_params", params_list ) != 0 ) {

		status = cis1_core::ERROR_CANT_READ_JOB_PARAMS_FILE;
		return 1;

	}

	return 0;
}
