#pragma once

#include "error_code.h"
#include "read_istream_kv_str.h"
#include "context_interface.h"
#include "session_interface.h"
#include "os_interface.h"

namespace cis1
{

void set_param(
        const cis1::context_interface& ctx,
        const session_interface& session,
        const std::string& param_name,
        const std::string& value,
        std::error_code& ec,
        const os_interface& os);

} // namespace cis1