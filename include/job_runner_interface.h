/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <functional>
#include <string>
#include <system_error>

namespace cis1
{

/**
 * \brief Abstracts jobs running
 */
struct job_runner_interface
{
    using on_exit_cb_t = std::function<void(
            std::error_code err,
            int exit_code)>;

    using on_line_read_cb_t = std::function<void(const std::string&)>;

    virtual ~job_runner_interface() = default;

    /**
     * \brief Starts job
     * @param[in] filename name of executable
     * @param[in] on_exit_cb will be called on exit
     * @param[in] on_out_line_read_cb will be called on new line in stdout
     * @param[in] on_err_line_read_cb will be called on new line in stderr
     */
    virtual void run(
            const std::string& filename,
            on_exit_cb_t&& on_exit_cb,
            on_line_read_cb_t&& on_out_line_read_cb,
            on_line_read_cb_t&& on_err_line_read_cb) = 0;
};

} // namespace cis1
