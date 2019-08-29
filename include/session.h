#pragma once

#include <optional>
#include <system_error>

#include "context_interface.h"
#include "os_interface.h"
#include "session_interface.h"

namespace cis1
{

class session
    : public session_interface
{
public:
    session(const std::string& session_id,
            bool opened_by_me);

    virtual bool opened_by_me() const override;

    virtual const std::string& session_id() const override;

private:
    const std::string session_id_;
    const bool opened_by_me_;
};

std::optional<session> invoke_session(
        context_interface& ctx,
        std::error_code& ec,
        const os_interface& os);

} // namespace cis1
