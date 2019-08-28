#include "read_istream_kv_str.h"

#include <algorithm>

#include "cis1_error_code.h"

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
            ec = cis1::error::error_code::invalid_kv_file_format;

            return;
        }
        else if(is.eof())
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
