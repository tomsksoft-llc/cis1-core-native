#include "fs_entry_adapter.h"

namespace cis1
{

fs_entry_adapter::fs_entry_adapter(const std::filesystem::directory_entry& entry)
    : entry_(entry)
{}

bool fs_entry_adapter::is_directory() const
{
    return entry_.is_directory();
}

std::filesystem::path fs_entry_adapter::path() const
{
    return entry_.path();
}

} // namespace cis1
