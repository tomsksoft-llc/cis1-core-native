/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <string>
#include <optional>

/**
 * \brief Validate whether this name is build name
 * @param[in] dir_name Name to check
 */
bool is_build(const std::string& dir_name);

/**
 * \brief Tries to convert string to uint32_t
 * \return Converted uint32_t if conversion possible or std::nullopt otherwise
 * @param[in] str String to convert
 */
std::optional<uint32_t> u32_from_string(const std::string& str);
