#pragma once

#include <sstream>

#include "ofstream_interface.h"

class ofstream_mock
    : public ofstream_interface
{
public:
    MOCK_CONST_METHOD0(is_open, bool());

    MOCK_METHOD0(ostream, std::ostream&());
};
