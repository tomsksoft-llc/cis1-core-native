#pragma once

#include <string>
#include <optional>

bool is_build(const std::string& dir_name);

std::optional<uint32_t> u32_from_string(const std::string& str);
