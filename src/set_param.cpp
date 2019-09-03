#include "set_param.h"

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
        read_istream_kv_str(session_prm_file->istream(), values, ec);
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

    bool found = false;
    for(auto& [k, v] : values)
    {
        if(k == param_name)
        {
            v = value;
            found = true;
        }
        session_prm_file->ostream() << k << '=' << v << '\n';
    }

    if(!found)
    {
        session_prm_file->ostream() << param_name << '=' << value << '\n';
    }
}

} // namespace cis1
