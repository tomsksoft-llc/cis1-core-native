/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include "ifstream_adapter.h"

namespace cis1
{

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

} // namespace cis1
