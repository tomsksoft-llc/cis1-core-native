/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include "set_param.h"

#include <cis1_proto_utils/param_codec.h>

namespace cis1
{

void set_param(
        const cis1::context_interface& ctx,
        const session_interface& session,
        const std::string& param_name,
        const std::string& value,
        std::error_code& ec,
        const os_interface& os)
{
    auto session_prm = ctx.base_dir() / "sessions" / (session.session_id() + ".prm");

    std::map<std::string, std::string> values;
    if(os.exists(session_prm, ec))
    {
        auto session_prm_file = os.open_ifstream(session_prm);
        if(!session_prm_file || !session_prm_file->is_open())
        {
            ec = cis1::error_code::cant_read_session_values_file;

            return;
        }
        proto_utils::read_istream_kv_str(session_prm_file->istream(), values, ec);
        if(ec)
        {
            ec = cis1::error_code::cant_read_session_values_file;

            return;
        }
    }

    auto session_prm_file = os.open_ofstream(session_prm, std::ios::trunc);
    if(!session_prm_file || !session_prm_file->is_open())
    {
        ec = cis1::error_code::cant_write_session_params_file;

        return;
    }

    const auto encoded_param_name = proto_utils::encode_param(param_name);
    const auto encoded_value = proto_utils::encode_param(value);

    bool found = false;
    for(auto& [k, v] : values)
    {
        if(k == encoded_param_name)
        {
            v = encoded_value;
            found = true;
        }
        session_prm_file->ostream() << k << '=' << v << '\n';
    }

    if(!found)
    {
        session_prm_file->ostream() << encoded_param_name << '=' << encoded_value << '\n';
    }
}

} // namespace cis1
