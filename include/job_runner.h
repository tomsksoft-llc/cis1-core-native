#pragma once

#include <filesystem>
#include <functional>

#include <boost/process/async_pipe.hpp>
#include <boost/process/environment.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <gtest/gtest_prod.h>

#include "os_interface.h"
#include "job_runner_interface.h"
#include "process.h"

#ifdef __linux__
    inline const std::string run_prefix{"./"};
#elif _WIN32
    inline const std::string run_prefix{"start "};
#else
#error "Unsupported platform."
#endif

class job_runner
    : public job_runner_interface
{
public:
    job_runner(
            boost::asio::io_context& ctx,
            boost::process::environment env,
            const std::filesystem::path& working_dir,
            const os_interface& os = os{});

    virtual void run(
            const std::string& filename,
            on_exit_cb_t&& on_exit_cb,
            on_line_read_cb_t&& on_line_read_cb) override;

    FRIEND_TEST(build_execute, job_runner);
private:
    template <class Process = process>
    void run_impl(
            const std::string& filename,
            on_exit_cb_t&& on_exit_cb,
            on_line_read_cb_t&& on_line_read_cb,
            const Process& p = {})
    {
        on_line_read_cb_ = std::move(on_line_read_cb);

        p.async_system(
                ctx_,
                std::move(on_exit_cb),
                run_prefix + filename,
                env_,
                working_dir_,
                p.std_in().close(),
                p.std_out() > out_pipe_,
                p.std_err().close());

        boost::asio::async_read_until(
                out_pipe_,
                streambuf_,
                '\n',
                boost::bind(
                        &job_runner::on_line_read,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
    boost::asio::io_context& ctx_;
    boost::process::async_pipe out_pipe_;
    boost::process::environment env_;
    std::filesystem::path working_dir_;
    boost::asio::streambuf streambuf_;
    on_line_read_cb_t on_line_read_cb_;
    const os_interface& os_;

    void on_line_read(
            const boost::system::error_code& error,
            std::size_t bytes_transferred);
};
