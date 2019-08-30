#pragma once

#include <istream>

namespace cis1
{

struct ifstream_interface
{
    virtual ~ifstream_interface() = default;

    virtual bool is_open() const = 0;

    virtual std::istream& istream() = 0;
};

} // namespace cis1