/*! \file cis1_core.cpp
    \brief Class cis1_core header.

Class cis1_core header file.

*/
#pragma once

#include <system_error>
#include <string>
#include <map>
#include <filesystem>

#include <boost/process.hpp>

/*! \class cis1_core
    \brief The cis1_core class incapsulate all cis1 core functionality.

    The Init must be called before using any other function.
*/

class cis1_core
{
public:
    /*! \enum cis1_core object statuses
        \brief cis1_core object statuses
    */
    enum class status
    {
        ok,
        not_init,
        error,
    };

    cis1_core();
    ~cis1_core();

    /*! \fn init
            \brief Initialize internal state for next works
            @return void
    */
    void init(std::error_code& ec);


    /*! \fn getstatus
        \brief Return current status of the object
        @return status Current status
    */
    status get_status();

    void invoke_session(std::error_code& ec);
    bool session_opened_by_me();
    std::string get_session_id();

    void startjob(
            const std::string& job_name,
            int& exit_code,
            std::error_code& ec);

    void setparam(
            const std::string& param_name,
            const std::string& param_value,
            std::error_code& ec);
    void getparam(
            const std::string& param_name,
            std::string& param_value,
            std::error_code& ec);

    void setvalue(
            const std::string& value_name,
            const std::string& value,
            std::error_code& ec);
    void getvalue(
            const std::string& value_name,
            std::string& value,
            std::error_code& ec);

private:
    status status_ = status::not_init;
    bool init_ = false;

    boost::process::environment env_;
    std::filesystem::path cis_base_dir_;

    std::string session_id_;
    bool session_opened_by_me_ = false;

    std::string get_env_var(const std::string& var_name);
    std::string get_new_build_dir(const std::filesystem::path& dir);

    void read_job_params(std::map<std::string, std::string>& params);
    void write_job_params(
            const std::filesystem::path& path,
            const std::map<std::string, std::string>& job_params,
            std::error_code& ec);

    int execute_job(
            const std::filesystem::path& path,
            const std::string& script,
            std::error_code& ec);

    bool read_file_str(
            const std::filesystem::path& path,
            std::map<std::string, std::string>& result);
};
