/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <filesystem>

namespace cis1
{

/**
 * \brief Interface for entriy in filesystem
 */
struct fs_entry_interface
{
    virtual ~fs_entry_interface() = default;

    /**
     * \brief Checks whether entry is directory
     * \return true if entry is directory
     */
    virtual bool is_directory() const = 0;

    /**
     * \brief Getter for directory path
     * \return directory path
     */
    virtual std::filesystem::path path() const = 0;
};

} // namespace cis1
