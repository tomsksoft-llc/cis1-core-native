#pragma once

#include <functional>
#include <filesystem>
#include <string>

#include <boost/system/error_code.hpp>
#include <boost/process.hpp>
#include <boost/asio.hpp>

namespace cis1
{

class process
{
public:
    template <class StdIn, class StdOut, class StdErr>
    void async_system(
            boost::asio::io_context& io_ctx,
            std::function<void(
                    boost::system::error_code err,
                    int exit_code)>&& cb,
            std::string cmd,
            boost::process::environment env,
            std::filesystem::path path,
            StdIn std_in,
            StdOut std_out,
            StdErr std_err) const
    {
        boost::process::async_system(
                io_ctx,
                cb,
                cmd,
                env,
                boost::process::start_dir = path.string(),
                std_in,
                std_out,
                std_err);
    }

    auto std_in() const
    {
        return boost::process::std_in;
    }

    auto std_out() const
    {
        return boost::process::std_out;
    }

    auto std_err() const
    {
        return boost::process::std_err;
    }
};

} // namespace cis1
