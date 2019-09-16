#pragma once

#include <ostream>

namespace cis1
{

/**
 * \brief Interface for ofstream
 */
struct ofstream_interface
{
    virtual ~ofstream_interface() = default;

    virtual bool is_open() const = 0;

    virtual std::ostream& ostream() = 0;
};

} // namespace cis1
