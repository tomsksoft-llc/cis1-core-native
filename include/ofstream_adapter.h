#pragma once

#include <filesystem>
#include <fstream>

#include "ofstream_interface.h"

class ofstream_adapter
    : public ofstream_interface
{
public:
    ofstream_adapter(
            const std::filesystem::path& filename,
            std::ios_base::openmode mode = std::ios_base::out);
    virtual bool is_open() const override;
    virtual std::ostream& ostream() override;
private:
    std::ofstream ofs_;
};
