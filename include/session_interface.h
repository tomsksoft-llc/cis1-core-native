#pragma once

#include <string>

namespace cis1
{

struct session_interface
{
    virtual ~session_interface() = default;

    virtual bool opened_by_me() const = 0;

    virtual const std::string& session_id() const = 0;
};

} // namespace cis1
