#pragma once

#include <filesystem>
#include <fstream>

#include "ifstream_interface.h"

namespace cis1
{

/**
 * \brief Adapts std::ifstream to ifstream_interface
 */
class ifstream_adapter
    : public ifstream_interface
{
public:
    /**
     * \brief Constructs ifstream_adapter instance
     * @param[in] filename Path to opening file
     * @param[in] mode File open mode. std::ios_base::in by default
     */
    ifstream_adapter(
            const std::filesystem::path& filename,
            std::ios_base::openmode mode = std::ios_base::in);

    /**
     * \brief Checks whether file open
     * \return true if file open false otherwise
     */
    virtual bool is_open() const override;

    /**
     * \brief Getter for istream
     * \return std::istream& to corresponding file istream
     */
    virtual std::istream& istream() override;

private:
    std::ifstream ifs_;
};

} // namespace cis1
