/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <system_error>
#include <string>

namespace cis1
{

enum class error_code
{
    ok,
    bad_state,
    base_dir_not_defined,
    base_dir_doesnt_exist,
    job_dir_doesnt_exist,
    cant_read_base_conf_file,
    cant_read_job_conf_file,
    cant_read_job_params_file,
    cant_read_session_params_file,
    cant_write_session_params_file,
    cant_read_session_values_file,
    script_doesnt_exist,
    cant_open_session,
    cant_open_build_output_file,
    cant_open_build_exit_code_file,
    cant_write_job_params_file,
    cant_write_job_conf_file,
    cant_write_session_id_file,
    cant_read_crons_file,
    cant_write_crons_file,
    invalid_cron_expression,
    cant_generate_build_num,
    cant_create_build_dir,
    cant_copy_script,
    cant_execute_script,
    invalid_kv_file_format,
    script_is_not_executable,
};

std::error_code make_error_code(error_code ec);

struct error_category
        : public std::error_category
{
    const char * name() const noexcept override;
    std::string message(int ev) const override;
};

} // namespace cis1

namespace std
{

template<>
struct is_error_code_enum<cis1::error_code>
    : public true_type
{};

} // namespace std
