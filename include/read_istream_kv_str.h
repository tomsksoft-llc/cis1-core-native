#pragma once

#include <istream>
#include <map>
#include <string>
#include <system_error>

#include "os_interface.h"

namespace cis1
{

/**
 * \brief Function for reading 'key=value' from istream
 * @param[in] is
 * @param[out] lines
 * @param[out] ec
 */
void read_istream_kv_str(
        std::istream& is,
        std::map<std::string, std::string>& lines,
        std::error_code& ec);

} // namespace cis1
