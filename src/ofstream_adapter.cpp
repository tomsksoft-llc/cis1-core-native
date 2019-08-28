#include "ofstream_adapter.h"

ofstream_adapter::ofstream_adapter(
        const std::filesystem::path& filename,
        std::ios_base::openmode mode)
    : ofs_(filename, mode)
{}

bool ofstream_adapter::is_open() const
{
    return ofs_.is_open();
}

std::ostream& ofstream_adapter::ostream()
{
    return ofs_;
}
