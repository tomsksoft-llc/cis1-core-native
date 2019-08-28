#pragma once

#include <istream>
#include <map>
#include <string>
#include <system_error>

#include "os_interface.h"

void read_istream_kv_str(
        std::istream& is,
        std::map<std::string, std::string>& lines,
        std::error_code& ec);
