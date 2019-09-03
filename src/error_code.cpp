#include "error_code.h"

namespace cis1
{

const char * error_category::name() const noexcept
{
    return "cis1::error";
}

std::string error_category::message(int ev) const
{
    switch(static_cast<error_code>(ev))
    {
        case error_code::ok:
            return "OK";

        case error_code::bad_state:
            return "Insufficient core state";

        case error_code::base_dir_not_defined:
            return "CIS base dir not defined";

        case error_code::base_dir_doesnt_exist:
            return "CIS base dir doesnt exist";

        case error_code::job_dir_doesnt_exist:
            return "Job dir doesnt exist";

        case error_code::cant_read_base_conf_file:
            return "Cant cis configuration file";

        case error_code::cant_read_job_conf_file:
            return "Cant read job configuration file";

        case error_code::cant_read_job_params_file:
            return "Cant read job params file";

        case error_code::cant_read_session_params_file:
            return "Cant read job params file";

        case error_code::cant_read_session_values_file:
            return "Cant read sessions values file";

        case error_code::script_doesnt_exist:
            return "Job script doesnt exist";

        case error_code::cant_open_session:
            return "Cant open session";

        case error_code::cant_open_build_output_file:
            return "Cant open build output file";

        case error_code::cant_open_build_exit_code_file:
            return "Cant open build exit_code file";

        case error_code::cant_write_job_params_file:
            return "Cant write job params file";

        case error_code::cant_write_job_conf_file:
            return "Cant write job conf file";

        case error_code::cant_write_session_id_file:
            return "Cant write job session_id file";

        case error_code::cant_generate_build_num:
            return "Cant generate build number";

        case error_code::cant_create_build_dir:
            return "Cant create build directory";

        case error_code::cant_copy_script:
            return "Cant copy job script";

        case error_code::cant_execute_script:
            return "Cant execute job script";

        case error_code::invalid_kv_file_format:
            return "Cant parse kv-file";

        default:
            return "(unrecognized error)";
    }
}

const error_category category;

std::error_code make_error_code(error_code e)
{
    return {static_cast<int>(e), category};
}

} // namespace cis1
