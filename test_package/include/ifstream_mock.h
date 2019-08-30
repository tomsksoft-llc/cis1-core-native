#pragma once

#include <sstream>

#include <gmock/gmock.h>

#include "ifstream_interface.h"

class ifstream_mock
    : public cis1::ifstream_interface
{
public:
    MOCK_CONST_METHOD0(is_open, bool());

    MOCK_METHOD0(istream, std::istream&());
};
