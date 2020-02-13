/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2020 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <string_view>
#include <optional>
#include <error_code.h>

namespace cis1
{

/**
 * Converts the given input string to an URL encoded string.
 * @param src - input string.
 * @return - URL encoded string.
 */
std::string url_encode(std::string_view src);

/**
 * Decode the given input string from an URL encoded string.
 * @param src - input string.
 * @param src - output string.
 * @return - true if the given input string has been parsed correct
 *           else false.
 */
bool url_decode(std::string_view src, std::string& dst);

} // namespace cis1
