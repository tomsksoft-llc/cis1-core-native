/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include "get_value.h"

#include <cis1_proto_utils/param_codec.h>

namespace cis1
{

std::optional<std::string> get_value(
        const cis1::context_interface& ctx,
        const session_interface& session,
        const std::string& value_name,
        std::error_code& ec,
        const os_interface& os)
{
    auto session_dat = ctx.base_dir() / "sessions" / (session.session_id() + ".dat");

    std::map<std::string, std::string> values;
    if(os.exists(session_dat, ec))
    {
        auto session_dat_file = os.open_ifstream(session_dat);
        if(!session_dat_file || !session_dat_file->is_open())
        {
            ec = cis1::error_code::cant_read_session_values_file;

            return std::nullopt;
        }
        read_istream_kv_str(session_dat_file->istream(), values, ec);
        if(ec)
        {
            ec = cis1::error_code::cant_read_session_values_file;

            return std::nullopt;
        }
    }

    const auto encoded_value_name = proto_utils::encode_param(value_name);
    if(auto it = values.find(encoded_value_name); it != values.end())
    {
        std::string decoded_value;
        if(!proto_utils::decode_param(it->second, decoded_value))
        {
            ec = cis1::error_code::cant_read_job_params_file;
            return std::nullopt;
        }

        return decoded_value;
    }

    return "";
}

} // namespace cis1
