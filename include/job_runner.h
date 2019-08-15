#pragma once

#include <filesystem>
#include <functional>

#include <boost/process/async_pipe.hpp>
#include <boost/process/environment.hpp>
#include <boost/asio/streambuf.hpp>

class job_runner
{
public:
    using on_exit_cb_t = std::function<void(
            boost::system::error_code err,
            int exit_code)>;
    using on_line_read_cb_t = std::function<void(const std::string&)>;

    job_runner(
            boost::asio::io_context& ctx,
            boost::process::environment env,
            const std::filesystem::path& working_dir);

    void run(
            const std::string& filename,
            on_exit_cb_t&& on_exit_cb,
            on_line_read_cb_t&& on_line_read_cb);
private:
    boost::asio::io_context& ctx_;
    boost::process::async_pipe out_pipe_;
    boost::process::environment env_;
    std::filesystem::path working_dir_;
    boost::asio::streambuf streambuf_;
    on_line_read_cb_t on_line_read_cb_;

    void on_line_read(
            const boost::system::error_code& error,
            std::size_t bytes_transferred);
};
