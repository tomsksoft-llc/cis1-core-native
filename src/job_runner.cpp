#include "job_runner.h"

#include <functional>
#include <vector>

#include <boost/process/async_system.hpp>
#include <boost/process/start_dir.hpp>
#include <boost/process/io.hpp>
#include <boost/process/env.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

namespace cis1
{

job_runner::job_runner(
        boost::asio::io_context& ctx,
        boost::process::environment env,
        const std::filesystem::path& working_dir,
        const os_interface& os)
    : ctx_(ctx)
    , out_pipe_(ctx)
    , env_(env)
    , working_dir_(working_dir)
    , os_(os)
{}

void job_runner::run(
        const std::string& filename,
        on_exit_cb_t&& on_exit_cb,
        on_line_read_cb_t&& on_line_read_cb)
{
    run_impl(
            filename,
            std::move(on_exit_cb),
            std::move(on_line_read_cb));
}

std::string make_string(boost::asio::streambuf& streambuf, size_t size)
{
    return {buffers_begin(streambuf.data()),
            buffers_begin(streambuf.data()) + size};
}

void job_runner::on_line_read(
        const boost::system::error_code& ec,
        std::size_t bytes_transferred)
{
    if(!ec)
    {
        on_line_read_cb_(
                make_string(streambuf_, bytes_transferred - 1));

        streambuf_.consume(bytes_transferred);
    }
    else if(ec == boost::asio::error::eof)
    {
        on_line_read_cb_(
                make_string(streambuf_, streambuf_.size()));

        streambuf_.consume(streambuf_.size());
    }

    if(!ec)
    {
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
}

} // namespace cis1
