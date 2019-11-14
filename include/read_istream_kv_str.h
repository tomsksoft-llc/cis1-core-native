/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <istream>
#include <map>
#include <vector>
#include <string>
#include <system_error>

#include "os_interface.h"

namespace cis1
{

/**
 * \brief Function for reading 'key=value' from istream to map
 * @param[in] is
 * @param[out] lines
 * @param[out] ec
 */
void read_istream_kv_str(
        std::istream& is,
        std::map<std::string, std::string>& lines,
        std::error_code& ec);

/**
 * \brief Function for reading 'key=value' from istream to ordered vector
 * @param[in] is
 * @param[out] lines
 * @param[out] ec
 */
void read_istream_ordered_kv_str(
        std::istream& is,
        std::vector<std::pair<std::string, std::string>>& lines,
        std::error_code& ec);

} // namespace cis1
