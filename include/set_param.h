/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <cis1_proto_utils/read_istream_kv_str.h>
#include "error_code.h"
#include "context_interface.h"
#include "session_interface.h"
#include "os_interface.h"

namespace cis1
{

/**
 * \brief Function that sets param for the next job
 * @param[in] ctx
 * @param[in] session
 * @param[in] param_name
 * @param[in] value
 * @param[out] ec
 * @param[in] os
 */
void set_param(
        const cis1::context_interface& ctx,
        const session_interface& session,
        const std::string& param_name,
        const std::string& value,
        std::error_code& ec,
        const os_interface& os);

} // namespace cis1
