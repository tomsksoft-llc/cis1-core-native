#include "utils.h"

#include <regex>

bool is_build(const std::string& dir_name)
{
    static const std::regex build_mask("^\\d{6}$");
    return std::regex_match(dir_name, build_mask);
}

std::optional<uint32_t> u32_from_string(const std::string& str)
{
    try
    {
        return stoul(str);
    }
    catch(...)
    {
        return std::nullopt;
    }
}
