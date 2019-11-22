/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include "os_interface.h"

namespace cis1
{

/**
 * \brief Default os calls implementation
 */
class os
    : public os_interface
{
public:
    /**
     * \brief Makes clone of current os instance
     * \return Cloned os instance
     */
     std::unique_ptr<os_interface> clone() const override;

    /**
     * \brief Environment var getter
     * \return Environment var string if var exists empty string otherwise
     * @param[in] name Variable name
     */
     std::string get_env_var(
            const std::string& name) const override;

    /**
     * \brief Checks whether dir is directory
     * @param[in] dir Path to fs entry
     * @param[out] ec
     */
     bool is_directory(
            const std::filesystem::path& dir,
            std::error_code& ec) const override;

    /**
     * \brief Check whether fs entry exists
     * @param[in] path Path to entry
     * @param[out] ec
     */
     bool exists(
            const std::filesystem::path& path,
            std::error_code& ec) const override;

    /**
     * \brief Getter for directory entries
     * \return Array of fs_entries in directory
     * @param[in] path Path to directory
     */
     std::vector<
            std::unique_ptr<fs_entry_interface>> list_directory(
            const std::filesystem::path& path) const override;

    /**
     * \brief Makes new directory
     * \return true is directory created successfully false otherwise
     * @param[in] dir Path to directory
     * @param[out] ec
     */
     bool create_directory(
            const std::filesystem::path& dir,
            std::error_code& ec) const override;

    /**
     * \brief Copies fs entry
     * @param[in] from Path to source fs entry
     * @param[in] to Path to destination fs entry
     * @param[out] ec
     */
     void copy(
            const std::filesystem::path& from,
            const std::filesystem::path& to,
            std::error_code& ec) const override;

    /**
     * \brief Open file for reading
     * @param[in] path Path to file
     * @param[in] mode Open mode default is std::ios_base::in
     */
     std::unique_ptr<ifstream_interface> open_ifstream(
            const std::filesystem::path& path,
            std::ios_base::openmode mode) const override;

    /**
     * \brief Open file for writing
     * @param[in] path Path to file
     * @param[in] mode Open mode default is std::ios_base::out
     */
     std::unique_ptr<ofstream_interface> open_ofstream(
            const std::filesystem::path& path,
            std::ios_base::openmode mode = std::ios_base::out) const override;

    /**
     * \brief Creates child process and detaches it
     * @param[in] start_dir Dir where process will be executed
     * @param[in] executable
     * @param[in] args Args passed to process
     * @param[in] env Environment passed to process
     */
    void spawn_process(
            const std::string& start_dir,
            const std::string& executable,
            const std::vector<std::string>& args,
            boost::process::environment env) const override;

    /**
     * \brief Remove fs entry (except for non-empty dir)
     * @param[in] path Path to fs entry
     * @param[out] ec
     */
    void remove(
            const std::filesystem::path& path,
            std::error_code& ec) const override;

    /**
     * \brief Remove fs entry
     * @param[in] path Path to fs entry
     * @param[out] ec
     */
    void remove_all(
            const std::filesystem::path& path,
            std::error_code& ec) const override;

    /**
     * \brief Checks fs entry is executable
     * @param[in] path Path to fs entry
     * @param[out] ec
     */
    bool is_executable(
            const std::filesystem::path& path,
            std::error_code& ec) const override;

    /**
     * \brief Makes fs entry executable
     * @param[in] path Path to fs entry
     * @param[out] ec
     */
    void make_executable(
            const std::filesystem::path& path,
            std::error_code& ec) const override;
};

} // namespace cis1
