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

class build
{
public:
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

    std::map<std::string, std::string>& params();

    void prepare_params(
            const context_interface& ctx,
            const session_interface& session,
            std::error_code& ec);

    void prepare_build_dir(std::error_code& ec);

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

std::optional<build> prepare_build(
        context_interface& ctx,
        std::string job_name,
        std::error_code& ec,
        const os_interface& os);

} // namespace cis1
