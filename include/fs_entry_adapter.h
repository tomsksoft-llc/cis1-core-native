#pragma once

#include "fs_entry_interface.h"

namespace cis1
{

/**
 * \brief Adapts std::filesystem::directory_entry to fs_entry_interface
 */
class fs_entry_adapter
    : public fs_entry_interface
{
public:
    /**
     * \brief Constructs fs_entry_adapter
     * @param[in] entry Directory entry to adapt
     */
    fs_entry_adapter(const std::filesystem::directory_entry& entry);

    /**
     * \brief Checks whether entry is directory
     * \return true if entry is directory
     */
    virtual bool is_directory() const override;

    /**
     * \brief Getter for directory path
     * \return directory path
     */
    virtual std::filesystem::path path() const override;

private:
    std::filesystem::directory_entry entry_;
};

} // namespace cis1
