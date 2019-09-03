#pragma once

#include <gmock/gmock.h>

#include "job_runner_interface.h"

class job_runner_mock
    : public cis1::job_runner_interface
{
public:
    void run(
            const std::string& filename,
            on_exit_cb_t&& on_exit_cb,
            on_line_read_cb_t&& on_out_line_read_cb,
            on_line_read_cb_t&& on_err_line_read_cb)
    {
        run_impl(
                filename,
                on_exit_cb,
                on_out_line_read_cb,
                on_err_line_read_cb);
    }

    MOCK_METHOD4(
            run_impl,
            void(   const std::string& filename,
                    on_exit_cb_t& on_exit_cb,
                    on_line_read_cb_t& on_out_line_read_cb,
                    on_line_read_cb_t& on_err_line_read_cb));
};

class job_runner_factory_mock
{
public:
    std::unique_ptr<cis1::job_runner_interface> operator()(
            boost::asio::io_context& ctx,
            boost::process::environment env,
            const std::filesystem::path& working_dir,
            const cis1::os_interface& os) const
    {
        return call_operator(ctx, env, working_dir, os);
    }

    MOCK_CONST_METHOD4(
            call_operator,
            std::unique_ptr<cis1::job_runner_interface>(
                    boost::asio::io_context& ctx,
                    boost::process::environment env,
                    const std::filesystem::path& working_dir,
                    const cis1::os_interface& os));
};
