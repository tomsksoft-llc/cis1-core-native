#pragma once

#include <optional>
#include <system_error>
#include <map>
#include <string>

#include <gtest/gtest_prod.h>

#include "os_interface.h"
#include "context_interface.h"
#include "session_interface.h"
#include "job_runner.h"

namespace cis1
{

/**
 * \brief Represents all build-related data and methods
 */
class build
{
public:
    /**
    * \brief job_runner_interface factory interface
    */
    using job_runner_factory_t =
            std::function<std::unique_ptr<job_runner_interface>(
                    boost::asio::io_context& ctx,
                    boost::process::environment env,
                    const std::filesystem::path& working_dir,
                    const os_interface& os)>;

    build(  const std::string& job_name,
            const std::filesystem::path& job_dir,
            const std::filesystem::path& script,
            const std::map<std::string, std::string>& params,
            const os_interface& os);

    /**
     * \brief Method for modifying params (e.g. from user input)
     * \return reference to params field
     */
    std::map<std::string, std::string>& params();

    /**
     * \brief Merge default params and session or user ones
     * @param[in] ctx
     * @param[in] session
     * @param[out] ec
     */
    void prepare_params(
            const context_interface& ctx,
            const session_interface& session,
            std::error_code& ec);

    /**
     * \brief Copies and writes files to build_dir
     * @param[in] session
     * @param[out] ec
     */
    void prepare_build_dir(
            const session_interface& session,
            std::error_code& ec);

    /**
     * \brief Executes job and set exit_code on success
     * @param[in] ctx
     * @param[out] ec
     * @param[out] exit_code Code returned by executed job
     * @param[in] job_runner_factory
     */
    void execute(
            context_interface& ctx,
            std::error_code& ec,
            int& exit_code,
            job_runner_factory_t job_runner_factory =
                            [](auto&&... args)
                            {
                                return std::make_unique<job_runner>(
                                        std::forward<decltype(args)>(args)...);
                            });

    /**
     * \brief Getter for build number string
     * \return build number as string (e.g. "000001")
     */
    std::string build_num();

private:
    const std::string job_name_;
    const std::filesystem::path job_dir_;
    const std::filesystem::path script_;
    std::filesystem::path build_dir_;
    std::map<std::string, std::string> params_;
    const os_interface& os_;

    void create_build_dir(std::error_code& ec);
};

/**
 * \brief Creates build if possible
 * \return valid build or std::nullopt
 * @param[in] ctx
 * @param[in] job_name Name of job which should be prepared to build
 * @param[out] ec
 * @param[in] os
 */
std::optional<build> prepare_build(
        context_interface& ctx,
        std::string job_name,
        std::error_code& ec,
        const os_interface& os);

} // namespace cis1
