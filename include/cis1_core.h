/*! \file cis1_core.cpp
	\brief Class cis1_core header.

Class cis1_core header file.

*/
#pragma once

#include <string>
#include <vector>



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

	enum class TStatus {
		OK,
		NOT_INIT,
		ERROR_ALREADY_INIT,
		ERROR_CIS_BASE_DIR_ENV_NOT_DEFINED,
		ERROR_CIS_BASE_DIR_NOTEXIST,
		ERROR_CANT_OPEN_SESSION,
		ERROR_CANT_EVAL_NEW_BUILD_NUM,
		ERROR_CANT_CREATE_BUILD_DIR,
		ERROR_CANT_COPY_SCRIPT_TO_BUILD_DIR,
		ERROR_CANT_EXECUTE_SCRIPT,
		ERROR_CANT_READ_JOB_PARAMS_FILE
	};



	cis1_core() {
	        status = TStatus::NOT_INIT;
	        cis_base_dir = "";
	        session_id = "";
        	session_opened_by_me_flag = false;
	}

	~cis1_core() {
		// TODO closesession to sessionlog/corelog ???
	};


	/*! \fn init
	        \brief Initialize internal state for next works
        	@return cis1_core::TStatus initialization result
	*/

	TStatus init();


	/*! \fn getstatus
		\brief Return current status of the object
		@return TStatus Current status
	*/

	TStatus getstatus() {
		return status;
	}


	std::string getstatus_str();


	bool session_opened_by_me() {
		return session_opened_by_me_flag;
	};


	std::string get_session_id() {
		return session_id;
	}


	int invoke_session();
	int get_job_params_list(
            const std::string& job_name,
            std::vector<std::string>& params_list ); // TODO change params type to map

	int startjob(const std::string& job_name, int& exit_code);

	int setparam(const std::string& param_name, const std::string& param_value);
	int getparam(const std::string& param_name, std::string& param_value);

	int setvalue(const std::string& value_name, const std::string& value);
	int getvalue(const std::string& value_name, std::string& value);

private:

	TStatus status;

	std::string cis_base_dir;

	std::string session_id;
	bool session_opened_by_me_flag;


	// OS depended functions

	std::string get_env_var( const std::string& var_name );
	int set_env_var( const std::string& var_name, const std::string& value );
	int is_dir( const std::string& dir );
	std::string get_new_build_dir( const std::string& dir );
	int create_dir(const std::string& dir);
	int copy_file(const std::string& src, const std::string& dst);
	int execute_script_in_dir(
            const std::string& dir,
            const std::string& script_file_name,
            const std::string& log_file_name,
            const std::string& exit_code_file_name,
            int& exit_code);
	int read_file_str(const std::string& fname, std::vector<std::string>& lines);
};
