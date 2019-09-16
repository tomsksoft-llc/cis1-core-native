#pragma once

#include <optional>
#include <string>

#include "error_code.h"
#include "read_istream_kv_str.h"
#include "context_interface.h"
#include "session_interface.h"
#include "os_interface.h"

namespace cis1
{

/**
 * \brief Function for retrieving param for current job
 * \return param or std::nullopt
 * @param[in] ctx
 * @param[in] session
 * @param[in] param_name
 * @param[out] ec
 * @param[in] os
 */
std::optional<std::string> get_param(
        const cis1::context_interface& ctx,
        const session_interface& session,
        const std::string& param_name,
        std::error_code& ec,
        const os_interface& os);

} // namespace cis1
