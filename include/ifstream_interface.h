#pragma once

#include <istream>

struct ifstream_interface
{
    virtual ~ifstream_interface() = default;
    virtual bool is_open() const = 0;
    virtual std::istream& istream() = 0;
};
