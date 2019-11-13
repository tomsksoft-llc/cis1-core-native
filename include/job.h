#pragma once

#include <map>
#include <optional>
#include <sstream>
#include <iomanip>

#include "context_interface.h"
#include "os_interface.h"
#include "job_runner.h"
#include "session_interface.h"

namespace cis1
{

/**
 * \brief Represents job-related data and methods
 */
class job
{
public:
    using job_runner_factory_t =
            std::function<std::unique_ptr<job_runner_interface>(
                    boost::asio::io_context& ctx,
                    boost::process::environment env,
                    const std::filesystem::path& working_dir,
                    const os_interface& os)>;

    /**
     * \brief Represents job.conf and job.params
     */
    struct config
    {
        std::string script;
        uint32_t keep_successful_builds;
        uint32_t keep_broken_builds;
        std::vector<std::pair<std::string, std::string>> params;
    };

    /**
     * \brief Allows to manage pending build
     */
    class build_handle
    {
    public:
        /**
         * \brief Constructs build handle
         * @param[in] job_arg reference to corresponding job
         * @param[in] number Number of build or std::nullopt
         *                   if build doesn't exists
         */
        build_handle(
                job& job_arg,
                std::optional<uint32_t> number);

        /**
         * \brief Executes pending build and move it to corresponding map
         * @param[in] ctx
         * @param[out] ec
         * @param[out] exit_code Job exit code (if job finished correctly)
         */
        void execute(
                cis1::context_interface& ctx,
                std::error_code& ec,
                int& exit_code);

        /**
         * \brief String getter for build number
         * \return String build number representation
         */
        std::string number_string();

        /**
         * \brief Getter for build number
         * \return Build number
         */
        uint32_t number();

        /**
         * \brief Bool conversion operator
         * \return true if handle is valid false otherwise
         */
        operator bool() const;

    private:
        job& job_;
        std::optional<uint32_t> number_;
    };

    /**
     * \brief Constructs job instance
     * @param[in] name Job name
     * @param[in] cfg Job config
     * @param[in] successful_builds Builds with exit_code == 0
     * @param[in] broken_builds Builds with exit_code != 0
     * @param[in] pending_builds Builds without exit_code
     * @param[in] os
     */
    job(    const std::string& name,
            const config& cfg,
            const std::map<uint32_t, std::filesystem::path>& successful_builds,
            const std::map<uint32_t, std::filesystem::path>& broken_builds,
            const std::map<uint32_t, std::filesystem::path>& pending_builds,
            cis1::os_interface& os);

    /**
     * \brief Clean job directory from outdated builds
     * @param[out] ec
     */
    void cleanup(std::error_code& ec);

    /**
     * \brief Getter for keep successful builds count
     * \return Count of successful builds to keep
     */
    uint32_t keep_successful_builds() const;

    /**
     * \brief Getter for keep broken builds count
     * \return Count of broken builds to keep
     */
    uint32_t keep_broken_builds() const;

    /**
     * \brief Getter for params
     * \return Ordered vector of job params
     */
    const std::vector<std::pair<std::string, std::string>>& params() const;

    /**
     * \brief Make pending build
     * @param[in] ctx
     * @param[in] session
     * @param[in] params Params for this build
     * @param[out] ec
     */
    build_handle prepare_build(
            const cis1::context_interface& ctx,
            const session_interface& session,
            const std::vector<std::pair<std::string, std::string>>& params,
            std::error_code& ec);

    /**
     * \brief Execute pending build
     * @param[in] build_number Number of build to execute
     * @param[in] ctx
     * @param[out] ec
     * @param[out] exit_code Build exit_code
     * @param[in] job_runner_factory
     */
    void execute(
            uint32_t build_number,
            cis1::context_interface& ctx,
            std::error_code& ec,
            int& exit_code,
            job_runner_factory_t job_runner_factory =
                            [](auto&&... args)
                            {
                                return std::make_unique<job_runner>(
                                        std::forward<decltype(args)>(args)...);
                            });

private:
    std::string name_;
    config config_;
    cis1::os_interface& os_;

    std::map<uint32_t, std::filesystem::path> successful_builds_;
    std::map<uint32_t, std::filesystem::path> broken_builds_;
    std::map<uint32_t, std::filesystem::path> pending_builds_;
};

/**
 * \brief Merge given params and session ones
 * @param[in] job Job name
 * @param[out] ec
 * @param[in] ctx
 * @param[in] os
 */
std::optional<job> load_job(
        const std::string& job,
        std::error_code& ec,
        cis1::context_interface& ctx,
        cis1::os_interface& os);

/**
 * \brief Merge given params and session ones
 * @param[in, out] params Params to merge
 * @param[in] os
 * @param[in] ctx
 * @param[in] session
 * @param[out] ec
 */
void prepare_params(
        std::vector<std::pair<std::string, std::string>>& params,
        const os_interface& os,
        const context_interface& ctx,
        const session_interface& session,
        std::error_code& ec);

} // namespace cis1
