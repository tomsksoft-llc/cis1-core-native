#pragma once

#include <optional>
#include <system_error>
#include <map>
#include <string>
#include <filesystem>

#include <boost/process.hpp>

#include "context_interface.h"
#include "os_interface.h"

namespace cis1
{

class context
    : public context_interface
{
public:
    context(const std::filesystem::path& base_dir,
            const std::map<std::string, std::string>& executables);

    virtual void set_env(
            const std::string& key,
            const std::string& val) override;

    virtual const boost::process::environment& env() const override;

    virtual const std::filesystem::path& base_dir() const override;

private:
    const std::filesystem::path base_dir_;
    const std::map<std::string, std::string> executables_;
    boost::process::environment env_;
};

std::optional<context> init_context(
        std::error_code& ec,
        const os_interface& os);

} // namespace cis1
