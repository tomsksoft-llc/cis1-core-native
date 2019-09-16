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

/// \cond DO_NOT_DOCUMENT
class build_execute_job_runner_Test;
/// \endcond

namespace cis1
{

/**
 * \brief Implementation of job_runner_interface
 */
class job_runner
    : public job_runner_interface
{
public:
    job_runner(
            boost::asio::io_context& ctx,
            boost::process::environment env,
            const std::filesystem::path& working_dir,
            const os_interface& os);

    /**
     * \brief Starts job
     * @param filename name of executable
     * @param on_exit_cb will be called on exit
     * @param on_out_line_read_cb will be called on new line in stdout
     * @param on_err_line_read_cb will be called on new line in stderr
     */
    virtual void run(
            const std::string& filename,
            on_exit_cb_t&& on_exit_cb,
            on_line_read_cb_t&& on_out_line_read_cb,
            on_line_read_cb_t&& on_err_line_read_cb) override;

    /// \cond DO_NOT_DOCUMENT
    FRIEND_TEST(::build_execute, job_runner);
    /// \endcond

private:
    boost::asio::io_context& ctx_;
    boost::process::async_pipe out_pipe_;
    boost::process::async_pipe err_pipe_;
    boost::process::environment env_;
    std::filesystem::path working_dir_;
    boost::asio::streambuf outbuf_;
    boost::asio::streambuf errbuf_;
    on_line_read_cb_t on_out_line_read_cb_;
    on_line_read_cb_t on_err_line_read_cb_;
    const os_interface& os_;

    template <class Process = process>
    void run_impl(
            const std::string& filename,
            on_exit_cb_t&& on_exit_cb,
            on_line_read_cb_t&& on_out_line_read_cb,
            on_line_read_cb_t&& on_err_line_read_cb,
            const Process& p = {})
    {
#ifdef __linux__
        const std::string run_prefix{"./"};
#elif _WIN32
        const std::string run_prefix{"start "};
#else
#error "Unsupported platform."
#endif
        on_out_line_read_cb_ = std::move(on_out_line_read_cb);
        on_err_line_read_cb_ = std::move(on_err_line_read_cb);

        p.async_system(
                ctx_,
                std::move(on_exit_cb),
                run_prefix + filename,
                env_,
                working_dir_,
                p.std_in().close(),
                p.std_out() > out_pipe_,
                p.std_err() > err_pipe_);

        boost::asio::async_read_until(
                out_pipe_,
                outbuf_,
                '\n',
                boost::bind(
                        &job_runner::on_out_line_read,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));

        boost::asio::async_read_until(
                err_pipe_,
                errbuf_,
                '\n',
                boost::bind(
                        &job_runner::on_err_line_read,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }

    void on_out_line_read(
            const boost::system::error_code& error,
            std::size_t bytes_transferred);

    void on_err_line_read(
            const boost::system::error_code& error,
            std::size_t bytes_transferred);
};

} // namespace cis1
