#pragma once

#include "cis1_error_code.h"
#include "read_istream_kv_str.h"
#include "context_interface.h"
#include "session_interface.h"
#include "os_interface.h"

void set_value(
        const cis1::context_interface& ctx,
        const session_interface& session,
        const std::string& value_name,
        const std::string& value,
        std::error_code& ec,
        const os_interface& os = os{})
{
    auto session_dat = ctx.base_dir() / "sessions" / (session.session_id() + ".dat");

    std::map<std::string, std::string> values;
    if(os.exists(session_dat, ec))
    {
        auto session_dat_file = os.open_ifstream(session_dat);
        if(!session_dat_file || !session_dat_file->is_open())
        {
            ec = cis1::error::error_code::cant_read_session_values_file;

            // TODO: corelog, session log

            return;
        }
        read_istream_kv_str(session_dat_file->istream(), values, ec);
        if(ec)
        {
            ec = cis1::error::error_code::cant_read_session_values_file;

            // TODO: corelog, session log

            return;
        }
    }

    auto session_dat_file = os.open_ofstream(session_dat, std::ios::trunc);
    if(!session_dat_file || !session_dat_file->is_open())
    {
        ec = cis1::error::error_code::cant_read_session_values_file;

        // TODO: corelog, session log

        return;
    }

    bool found = false;
    for(auto& [k, v] : values)
    {
        if(k == value_name)
        {
            v = value;
            found = true;
        }
        session_dat_file->ostream() << k << '=' << v << '\n';
    }

    if(!found)
    {
        session_dat_file->ostream() << value_name << '=' << value << '\n';
    }

    // TODO check if value exist in job.value

    // TODO read session prm file and replace the value value or add it to the prm file
}
