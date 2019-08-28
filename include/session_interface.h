#pragma once

#include <string>

struct session_interface
{
    virtual ~session_interface() = default;
    virtual bool opened_by_me() const = 0;
    virtual const std::string& session_id() const = 0;
};
