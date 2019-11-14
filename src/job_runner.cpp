/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

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
    , err_pipe_(ctx)
    , env_(env)
    , working_dir_(working_dir)
    , os_(os)
{}

void job_runner::run(
        const std::string& filename,
        on_exit_cb_t&& on_exit_cb,
        on_line_read_cb_t&& on_out_line_read_cb,
        on_line_read_cb_t&& on_err_line_read_cb)
{
    run_impl(
            filename,
            std::move(on_exit_cb),
            std::move(on_out_line_read_cb),
            std::move(on_err_line_read_cb));
}

std::string make_string(boost::asio::streambuf& streambuf, size_t size)
{
    return {buffers_begin(streambuf.data()),
            buffers_begin(streambuf.data()) + size};
}

void job_runner::on_out_line_read(
        const boost::system::error_code& ec,
        std::size_t bytes_transferred)
{
    if(!ec)
    {
        on_out_line_read_cb_(
                make_string(outbuf_, bytes_transferred - 1));

        outbuf_.consume(bytes_transferred);
    }
    else if(ec == boost::asio::error::eof)
    {
        on_out_line_read_cb_(
                make_string(outbuf_, outbuf_.size()));

        outbuf_.consume(outbuf_.size());
    }

    if(!ec)
    {
        boost::asio::async_read_until(
                out_pipe_,
                outbuf_,
                '\n',
                boost::bind(
                        &job_runner::on_out_line_read,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
}

void job_runner::on_err_line_read(
        const boost::system::error_code& ec,
        std::size_t bytes_transferred)
{
    if(!ec)
    {
        on_err_line_read_cb_(
                make_string(errbuf_, bytes_transferred - 1));

        errbuf_.consume(bytes_transferred);
    }
    else if(ec == boost::asio::error::eof && bytes_transferred != 0)
    {
        on_err_line_read_cb_(
                make_string(errbuf_, errbuf_.size()));

        errbuf_.consume(errbuf_.size());
    }

    if(!ec)
    {
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
}

} // namespace cis1
