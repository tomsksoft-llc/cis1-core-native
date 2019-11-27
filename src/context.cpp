/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include "context.h"

#include <map>
#include <string>
#include <memory>

#include "error_code.h"
#include "read_istream_kv_str.h"
#include "get_parent_id.h"
#include "utils.h"

namespace cis1
{

context::context(
        const std::filesystem::path& base_dir,
        const std::map<std::string, std::string>& executables)
    : base_dir_(base_dir)
    , executables_(executables)
    , env_(boost::this_process::environment())
    , process_id_(boost::this_process::get_id())
    , parent_startjob_id_(0)
{
    auto parent_startjob_id = get_env_var("parent_startjob_id");
    if(auto parent_startjob_id_opt = u32_from_string(parent_startjob_id);
            parent_startjob_id_opt)
    {
        parent_startjob_id_ = parent_startjob_id_opt.value();
    }

    for(auto& [k, v] : executables)
    {
        env_[k] = v;
    }
}

void context::set_env_var(
        const std::string& key,
        const std::string& val)
{
    env_[key] = val;
}

std::string context::get_env_var(
        const std::string& key)
{
    std::string var;

    if(auto it = env_.find(key); it != env_.end())
    {
        var = it->to_vector()[0];
    }

    return var;
}

const boost::process::environment& context::env() const
{
    return env_;
}

const std::filesystem::path& context::base_dir() const
{
    return base_dir_;
}

size_t context::process_id() const
{
    return process_id_;
}

size_t context::parent_startjob_id() const
{
    return parent_startjob_id_;
}

std::optional<context> init_context(
        std::error_code& ec,
        const os_interface& os)
{
    std::filesystem::path cis_base_dir = os.get_env_var("cis_base_dir");

    if(cis_base_dir.empty())
    {
        ec = cis1::error_code::base_dir_not_defined;

        return std::nullopt;
    }

    if(!os.is_directory(cis_base_dir, ec) || ec)
    {
        ec = cis1::error_code::base_dir_doesnt_exist;

        return std::nullopt;
    }

    auto is = os.open_ifstream(cis_base_dir / "core" / "cis.conf");
    if(!is || !is->is_open())
    {
        ec = cis1::error_code::cant_read_base_conf_file;

        return std::nullopt;
    }

    std::map<std::string, std::string> executables;
    read_istream_kv_str(
            is->istream(),
            executables,
            ec);

    if(ec)
    {
        ec = cis1::error_code::cant_read_base_conf_file;

        return std::nullopt;
    }

    return context{cis_base_dir, executables};
}

} // namespace cis1
