/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

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
 * \brief Function for retrieve value from current session
 * \return value or std::nullopt
 * @param[in] ctx
 * @param[in] session
 * @param[in] value_name
 * @param[out] ec
 * @param[in] os
 */
std::optional<std::string> get_value(
        const cis1::context_interface& ctx,
        const session_interface& session,
        const std::string& value_name,
        std::error_code& ec,
        const os_interface& os);

} // namespace cis1
