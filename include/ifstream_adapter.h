#pragma once

#include <filesystem>
#include <fstream>

#include "ifstream_interface.h"

class ifstream_adapter
    : public ifstream_interface
{
public:
    ifstream_adapter(
            const std::filesystem::path& filename,
            std::ios_base::openmode mode = std::ios_base::in);
    virtual bool is_open() const override;
    virtual std::istream& istream() override;
private:
    std::ifstream ifs_;
};
