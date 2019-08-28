#pragma once

#include <string>
#include <filesystem>

#include <boost/process.hpp>

namespace cis1
{

struct context_interface
{
    virtual ~context_interface() = default;
    virtual void set_env(const std::string& key, const std::string& val) = 0;
    virtual const boost::process::environment& env() const = 0;
    virtual const std::filesystem::path& base_dir() const = 0;
};

} // namespace cis1
