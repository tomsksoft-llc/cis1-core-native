#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include <istream>

#include <boost/process.hpp>

#include "ifstream_interface.h"
#include "ofstream_interface.h"
#include "fs_entry_interface.h"

namespace cis1
{

/**
 * \brief Interface for all os calls
 */
struct os_interface
{
    virtual ~os_interface() = default;

    virtual std::unique_ptr<os_interface> clone() const = 0;

    virtual std::string get_env_var(
            const std::string& name) const = 0;

    virtual bool is_directory(
            const std::filesystem::path& dir,
            std::error_code& ec) const = 0;

    virtual bool exists(
            const std::filesystem::path& path,
            std::error_code& ec) const = 0;

    /*
    virtual polymorphic_range<fs_entry_interface> directory_iterator(
            const std::filesystem::path& path) const = 0;
    */

    virtual std::vector<
            std::unique_ptr<fs_entry_interface>> list_directory(
            const std::filesystem::path& path) const = 0;

    virtual bool create_directory(
            const std::filesystem::path& dir,
            std::error_code& ec) const = 0;

    virtual void copy(
            const std::filesystem::path& from,
            const std::filesystem::path& to,
            std::error_code& ec) const = 0;

    virtual std::unique_ptr<ifstream_interface> open_ifstream(
            const std::filesystem::path& path,
            std::ios_base::openmode mode = std::ios_base::in) const = 0;

    virtual std::unique_ptr<ofstream_interface> open_ofstream(
            const std::filesystem::path& path,
            std::ios_base::openmode mode = std::ios_base::out) const = 0;
};

} // namespace cis1
