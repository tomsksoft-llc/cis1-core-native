#pragma once

#include "fs_entry_interface.h"

class fs_entry_adapter
    : public fs_entry_interface
{
public:
    fs_entry_adapter(const std::filesystem::directory_entry& entry);

    virtual bool is_directory() const override;

    virtual std::filesystem::path path() const override;
private:
    std::filesystem::directory_entry entry_;
};
