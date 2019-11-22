#pragma once

#include <gmock/gmock.h>

#include "session_interface.h"

class session_mock
    : public cis1::session_interface
{
public:
    MOCK_CONST_METHOD0(
            opened_by_me,
            bool());

    MOCK_CONST_METHOD0(
            session_id,
            const std::string&());

    MOCK_METHOD1(
            on_close,
            void(std::function<void(session_interface&)> handler));
};
