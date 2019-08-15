#pragma once

#include <system_error>

namespace cis1
{

namespace error
{

enum class error_code
{
    ok,
    bad_state,
    base_dir_not_defined,
    base_dir_doesent_exist,
    cant_read_base_conf_file,
    cant_read_job_conf_file,
    cant_read_job_params_file,
    cant_read_session_params_file,
    cant_read_session_values_file,
    script_doesnt_exist,
    cant_open_session,
    cant_generate_build_num,
    cant_create_build_dir,
    cant_copy_script,
    cant_execute_script,
};

std::error_code make_error_code(error_code ec);

struct error_category
        : public std::error_category
{
    const char * name() const noexcept override;
    std::string message(int ev) const override;
};

} // namespace error

} // namespace cis1

namespace std
{

template<>
struct is_error_code_enum<cis1::error::error_code>
    : public true_type
{};

} // namespace std
