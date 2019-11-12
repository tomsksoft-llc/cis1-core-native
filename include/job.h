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

class job
{
public:
    using job_runner_factory_t =
            std::function<std::unique_ptr<job_runner_interface>(
                    boost::asio::io_context& ctx,
                    boost::process::environment env,
                    const std::filesystem::path& working_dir,
                    const os_interface& os)>;

    struct config
    {
        std::string script;
        uint32_t keep_successful_builds;
        uint32_t keep_broken_builds;
        std::vector<std::pair<std::string, std::string>> params;
    };

    class build_handle
    {
    public:
        build_handle(
                job& job_arg,
                std::optional<uint32_t> number);

        void execute(
                cis1::context_interface& ctx,
                std::error_code& ec,
                int& exit_code);

        std::string number_string();

        uint32_t number();

        operator bool() const;

    private:
        job& job_;
        std::optional<uint32_t> number_;
    };

    job(    const std::string& name,
            const config& cfg,
            const std::map<uint32_t, std::filesystem::path>& successful_builds,
            const std::map<uint32_t, std::filesystem::path>& broken_builds,
            const std::map<uint32_t, std::filesystem::path>& pending_builds,
            cis1::os_interface& os);

    void cleanup(std::error_code& ec);

    uint32_t keep_successful_builds() const;
    uint32_t keep_broken_builds() const;

    const std::vector<std::pair<std::string, std::string>>& params() const;

    build_handle prepare_build(
            const cis1::context_interface& ctx,
            const session_interface& session,
            const std::vector<std::pair<std::string, std::string>>& params,
            std::error_code& ec);

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

std::optional<job> load_job(
        const std::string& str,
        std::error_code& ec,
        cis1::context_interface& ctx,
        cis1::os_interface& os);

void prepare_params(
        std::vector<std::pair<std::string, std::string>>& params,
        const os_interface& os,
        const context_interface& ctx,
        const session_interface& session,
        std::error_code& ec);

} // namespace cis1
