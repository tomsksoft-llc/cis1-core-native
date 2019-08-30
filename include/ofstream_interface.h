#pragma once

#include <istream>

namespace cis1
{

struct ofstream_interface
{
    virtual ~ofstream_interface() = default;

    virtual bool is_open() const = 0;

    virtual std::ostream& ostream() = 0;
};

} // namespace cis1
