#include "ifstream_adapter.h"

ifstream_adapter::ifstream_adapter(
        const std::filesystem::path& filename,
        std::ios_base::openmode mode)
    : ifs_(filename, mode)
{}

bool ifstream_adapter::is_open() const
{
    return ifs_.is_open();
}

std::istream& ifstream_adapter::istream()
{
    return ifs_;
}
