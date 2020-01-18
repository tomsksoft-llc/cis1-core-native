/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include "read_istream_kv_str.h"

#include <algorithm>

#include "error_code.h"

namespace cis1
{

void read_istream_kv_str(
        std::istream& is,
        std::map<std::string, std::string>& lines,
        std::error_code& ec)
{
    while(!is.eof())
    {
        std::string key;
        std::string val;

        std::getline(is, key, '=');

        if(key.empty() && is.good() && !is.eof())
        {
            ec = cis1::error_code::invalid_kv_file_format;

            return;
        }

        if(is.eof())
        {
            return;
        }

        std::getline(is, val, '\n');

        val.erase(
                std::find_if(
                        val.rbegin(),
                        val.rend(),
                        [](int ch)
                        {
                            return !std::isspace(ch);
                        }).base(),
                val.end());

        lines[key] = val;
    }
}

void read_istream_ordered_kv_str(
        std::istream& is,
        std::vector<std::pair<std::string, std::string>>& lines,
        std::error_code& ec)
{
    while(!is.eof())
    {
        std::string key;
        std::string val;

        std::getline(is, key, '=');

        if(key.empty() && is.good() && !is.eof())
        {
            ec = cis1::error_code::invalid_kv_file_format;

            return;
        }

        if(is.eof())
        {
            return;
        }

        std::getline(is, val, '\n');

        val.erase(
                std::find_if(
                        val.rbegin(),
                        val.rend(),
                        [](int ch)
                        {
                            return !std::isspace(ch);
                        }).base(),
                val.end());

        lines.emplace_back(key, val);
    }
}

} // namespace cis1
