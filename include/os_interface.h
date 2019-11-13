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

    /**
     * \brief Makes clone of current os instance
     * \return Cloned os instance
     */
    virtual std::unique_ptr<os_interface> clone() const = 0;

    /**
     * \brief Environment var getter
     * \return Environment var string if var exists empty string otherwise
     * @param[in] name Variable name
     */
    virtual std::string get_env_var(
            const std::string& name) const = 0;

    /**
     * \brief Checks whether dir is directory
     * @param[in] dir Path to fs entry
     * @param[out] ec
     */
    virtual bool is_directory(
            const std::filesystem::path& dir,
            std::error_code& ec) const = 0;

    /**
     * \brief Check whether fs entry exists
     * @param[in] path Path to entry
     * @param[out] ec
     */
    virtual bool exists(
            const std::filesystem::path& path,
            std::error_code& ec) const = 0;

    /*
    virtual polymorphic_range<fs_entry_interface> directory_iterator(
            const std::filesystem::path& path) const = 0;
    */

    /**
     * \brief Getter for directory entries
     * \return Array of fs_entries in directory
     * @param[in] path Path to directory
     */
    virtual std::vector<
            std::unique_ptr<fs_entry_interface>> list_directory(
            const std::filesystem::path& path) const = 0;

    /**
     * \brief Makes new directory
     * \return true is directory created successfully false otherwise
     * @param[in] dir Path to directory
     * @param[out] ec
     */
    virtual bool create_directory(
            const std::filesystem::path& dir,
            std::error_code& ec) const = 0;

    /**
     * \brief Copies fs entry
     * @param[in] from Path to source fs entry
     * @param[in] to Path to destination fs entry
     * @param[out] ec
     */
    virtual void copy(
            const std::filesystem::path& from,
            const std::filesystem::path& to,
            std::error_code& ec) const = 0;

    /**
     * \brief Open file for reading
     * @param[in] path Path to file
     * @param[in] mode Open mode default is std::ios_base::in
     */
    virtual std::unique_ptr<ifstream_interface> open_ifstream(
            const std::filesystem::path& path,
            std::ios_base::openmode mode = std::ios_base::in) const = 0;

    /**
     * \brief Open file for writing
     * @param[in] path Path to file
     * @param[in] mode Open mode default is std::ios_base::out
     */
    virtual std::unique_ptr<ofstream_interface> open_ofstream(
            const std::filesystem::path& path,
            std::ios_base::openmode mode = std::ios_base::out) const = 0;

    /**
     * \brief Creates child process and detaches it
     * @param[in] start_dir Dir where process will be executed
     * @param[in] executable
     * @param[in] args Args passed to process
     * @param[in] env Environment passed to process
     */
    virtual void spawn_process(
            const std::string& start_dir,
            const std::string& executable,
            const std::vector<std::string>& args,
            boost::process::environment env) const = 0;

    /**
     * \brief Remove fs entry (except for non-empty dir)
     * @param[in] path Path to fs entry
     * @param[out] ec
     */
    virtual void remove(
            const std::filesystem::path& path,
            std::error_code& ec) const = 0;

    /**
     * \brief Remove fs entry
     * @param[in] path Path to fs entry
     * @param[out] ec
     */
    virtual void remove_all(
            const std::filesystem::path& path,
            std::error_code& ec) const = 0;
};

} // namespace cis1
