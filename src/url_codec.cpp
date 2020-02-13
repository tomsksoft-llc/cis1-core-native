/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2020 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#include "url_codec.h"

#include <sstream>
#include <iomanip>

namespace cis1
{

std::optional<char> hex_str_to_char(std::string_view src)
{
    if(src.length() != 2)
    {
        return std::nullopt;
    }

    unsigned char result = 0;
    for(const auto ch : src)
    {
        unsigned char num = 0;
        if('0' <= ch && ch <= '9')
        {
            num = ch - '0';
        }
        else if('a' <= ch && ch <= 'f')
        {
            num = ch - 'a' + 10;
        }
        else if('A' <= ch && ch <= 'F')
        {
            num = ch - 'A' + 10;
        }
        else
        {
            return std::nullopt;
        }

        // 0xF - 2^4
        result <<= 4u;
        result += num;
    }
    return static_cast<char>(result);
}

std::string url_encode(std::string_view src)
{
    std::string result;
    result.reserve(src.size() * 3);
    for(const char ch : src)
    {
        if(('0' <= ch && ch <= '9') ||//0-9
           ('a' <= ch && ch <= 'z') ||//abc...xyz
           ('A' <= ch && ch <= 'Z') || //ABC...XYZ
           (ch == '-' || ch == '.' || ch == '_' || ch == '~')
                )
        {
            result.push_back(ch);
        }
        else
        {
            const auto num = static_cast<const unsigned int>(ch);

            //convert the character to %NN where NN is a two-digit hexadecimal number
            std::stringstream ss;
            ss << '%' << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (0xFFu & num);
            result.append(ss.str());
        }
    }

    return result;
}

bool url_decode(std::string_view src, std::string& dst)
{
    dst.clear();
    dst.reserve(src.length());

    std::size_t i = 0;
    while(i < src.length())
    {
        const char ch = src[i];
        if(ch != '%')
        {
            dst.push_back(ch);
            ++i;
            continue;
        }

        if(i + 2 >= src.length())
        {
            return false;
        }

        const auto hx_as_char = hex_str_to_char(src.substr(i + 1, 2));
        if(!hx_as_char)
        {
            return false;
        }

        dst.push_back(hx_as_char.value());
        i += 3;
    }

    return true;
}

}
