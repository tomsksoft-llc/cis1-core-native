#pragma once

#include <filesystem>

namespace cis1
{

struct fs_entry_interface
{
    virtual ~fs_entry_interface() = default;

    virtual bool is_directory() const = 0;

    virtual std::filesystem::path path() const = 0;
};

} // namespace cis1
