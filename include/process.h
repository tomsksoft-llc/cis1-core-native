/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <functional>
#include <filesystem>
#include <string>

#include <boost/system/error_code.hpp>
#include <boost/process.hpp>
#include <boost/asio.hpp>

namespace cis1
{

/**
 * \brief Default implementation for process-related things
 */
class process
{
public:
    /**
     * \brief Execute command asynchronously
     * @param[in] io_ctx
     * @param[in] cb Process finished callback
     * @param[in] cmd Command to run
     * @param[in] env Environment passed to process
     * @param[in] path Directory to run process
     * @param[in] std_in
     * @param[in] std_out
     * @param[in] std_err
     */
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

    /**
     * \brief Standard input stream
     * \return boost implementation of stdin
     */
    auto std_in() const
    {
        return boost::process::std_in;
    }

    /**
     * \brief Standard output stream
     * \return boost implementation of stdout
     */
    auto std_out() const
    {
        return boost::process::std_out;
    }

    /**
     * \brief Standard error stream
     * \return boost implementation of stderr
     */
    auto std_err() const
    {
        return boost::process::std_err;
    }
};

} // namespace cis1
