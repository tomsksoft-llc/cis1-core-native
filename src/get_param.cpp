#include "get_param.h"

namespace cis1
{

std::optional<std::string> get_param(
        const cis1::context_interface& ctx,
        const session_interface& session,
        const std::string& value_name,
        std::error_code& ec,
        const os_interface& os)
{
    auto session_prm = ctx.base_dir() / "jobs"
                        / os.get_env_var("job_name")
                        / os.get_env_var("build_number")
                        / "job.params";

    std::map<std::string, std::string> values;
    if(os.exists(session_prm, ec))
    {
        auto job_prm_file = os.open_ifstream(session_prm);
        if(!job_prm_file || !job_prm_file->is_open())
        {
            ec = cis1::error_code::cant_read_job_params_file;

            // TODO: corelog, session log

            return std::nullopt;
        }
        read_istream_kv_str(job_prm_file->istream(), values, ec);
        if(ec)
        {
            ec = cis1::error_code::cant_read_job_params_file;

            // TODO: corelog, session log

            return std::nullopt;
        }
    }

    if(auto it = values.find(value_name); it != values.end())
    {
        return it->second;
    }

    return "";
}

} // namespace cis1
