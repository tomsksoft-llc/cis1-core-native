#include "cis1_context.h"

#include <map>
#include <string>

#include "cis1_error_code.h"
#include "read_istream_kv_str.h"

namespace cis1
{

context::context(
        const std::filesystem::path& base_dir,
        const std::map<std::string, std::string>& executables)
    : base_dir_(base_dir)
    , executables_(executables)
    , env_(boost::this_process::environment())
{
    for(auto& [k, v] : executables)
    {
        env_[k] = v;
    }
}


void context::set_env(const std::string& key, const std::string& val)
{
    env_[key] = val;
}

const boost::process::environment& context::env() const
{
    return env_;
}

const std::filesystem::path& context::base_dir() const
{
    return base_dir_;
}

std::optional<context> init_context(
        std::error_code& ec,
        const os_interface& os)
{
    std::filesystem::path cis_base_dir = os.get_env_var("cis_base_dir");

    if(cis_base_dir.empty())
    {
        ec = cis1::error::error_code::base_dir_not_defined;

        return std::nullopt;
    }

    if(!os.is_directory(cis_base_dir, ec) || ec)
    {
        ec = cis1::error::error_code::base_dir_doesnt_exist;

        return std::nullopt;
    }

    auto is = os.open_ifstream(cis_base_dir / "core" / "cis.conf");
    if(!is || !is->is_open())
    {
        ec = cis1::error::error_code::cant_read_base_conf_file;

        return std::nullopt;
    }

    std::map<std::string, std::string> executables;
    read_istream_kv_str(
            is->istream(),
            executables,
            ec);

    if(ec)
    {
        ec = cis1::error::error_code::cant_read_base_conf_file;

        return std::nullopt;
    }

    // TODO: init corelog system

    // TODO: corelog

    // TODO: init sessionlog, log about new sessoin if need

    return context{cis_base_dir, executables};
}

} // namespace cis1
