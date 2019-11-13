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

    /**
     * \brief Checks whether ofstream open
     * \return true if open false otherwise
     */
    virtual bool is_open() const = 0;

    /**
     * \brief Getter for ostream
     * \return std::ostream& to corresponding ostream
     */
    virtual std::ostream& ostream() = 0;
};

} // namespace cis1
