#pragma once

#include <istream>

namespace cis1
{

/**
 * \brief Interface for ifstream
 */
struct ifstream_interface
{
    virtual ~ifstream_interface() = default;

    /**
     * \brief Checks whether ifstream open
     * \return true if open false otherwise
     */
    virtual bool is_open() const = 0;

    /**
     * \brief Getter for istream
     * \return std::istream& to corresponding istream
     */
    virtual std::istream& istream() = 0;
};

} // namespace cis1
