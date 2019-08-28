#pragma once

#include <istream>

struct ofstream_interface
{
    virtual ~ofstream_interface() = default;
    virtual bool is_open() const = 0;
    virtual std::ostream& ostream() = 0;
};
