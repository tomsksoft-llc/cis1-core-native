#include "get_value.h"

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

            // TODO: corelog, session log

            return std::nullopt;
        }
        read_istream_kv_str(session_dat_file->istream(), values, ec);
        if(ec)
        {
            ec = cis1::error_code::cant_read_session_values_file;

            // TODO: corelog, session log

            return std::nullopt;
        }
    }

    if(auto it = values.find(value_name); it != values.end())
    {
        return it->second;
    }

    // TODO check if value exist in job.value

    // TODO read session prm file and replace the value value or add it to the prm file

    return "";
}

} // namespace cis1
