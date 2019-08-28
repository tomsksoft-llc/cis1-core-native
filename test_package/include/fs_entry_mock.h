#pragma once

#include <gmock/gmock.h>

#include "fs_entry_interface.h"

class fs_entry_mock
    : public fs_entry_interface
{
public:
    MOCK_CONST_METHOD0(
            is_directory,
            bool());

    MOCK_CONST_METHOD0(
            path,
            std::filesystem::path());
};
