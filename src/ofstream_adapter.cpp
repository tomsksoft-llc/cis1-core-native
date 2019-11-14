/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include "ofstream_adapter.h"

namespace cis1
{

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

} // namespace cis1
