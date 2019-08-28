#pragma once

#include <functional>
#include <string>
#include <system_error>

struct job_runner_interface
{
    using on_exit_cb_t = std::function<void(
            std::error_code err,
            int exit_code)>;
    using on_line_read_cb_t = std::function<void(const std::string&)>;

    virtual ~job_runner_interface() = default;

    virtual void run(
            const std::string& filename,
            on_exit_cb_t&& on_exit_cb,
            on_line_read_cb_t&& on_line_read_cb) = 0;
};
