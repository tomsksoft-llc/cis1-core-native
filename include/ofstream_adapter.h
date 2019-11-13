#pragma once

#include <filesystem>
#include <fstream>

#include "ofstream_interface.h"

namespace cis1
{

/**
 * \brief Adapts std::ofstream to ofstream_interface
 */
class ofstream_adapter
    : public ofstream_interface
{
public:
    /**
     * \brief Constructs ofstream_adapter instance
     * @param[in] filename Path to opening file
     * @param[in] mode File open mode. std::ios_base::out by default
     */
    ofstream_adapter(
            const std::filesystem::path& filename,
            std::ios_base::openmode mode = std::ios_base::out);

    /**
     * \brief Checks whether file open
     * \return true if file open false otherwise
     */
    virtual bool is_open() const override;
    /**
     * \brief Getter for ostream
     * \return std::ostream& to corresponding file ostream
     */
    virtual std::ostream& ostream() override;

private:
    std::ofstream ofs_;
};

} // namespace cis1
