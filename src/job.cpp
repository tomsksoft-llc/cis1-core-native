/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include "job.h"

#include <sstream>
#include <iomanip>

#include "utils.h"
#include "read_istream_kv_str.h"
#include "error_code.h"

namespace cis1
{

job::build_handle::build_handle(
        job& job_arg,
        std::optional<uint32_t> number)
    : job_(job_arg)
    , number_(number)
{}

void job::build_handle::execute(
        cis1::context_interface& ctx,
        std::error_code& ec,
        int& exit_code)
{
    job_.execute(
            number_.value(),
            ctx,
            ec,
            exit_code);
}

std::string job::build_handle::number_string()
{
    std::stringstream ss;

    ss << std::setfill('0')
       << std::setw(6) << number_.value();

    return ss.str();
}

uint32_t job::build_handle::number()
{
    return number_.value();
}

job::build_handle::operator bool() const
{
    return static_cast<bool>(number_);
}

job::job(
        const std::string& name,
        const config& cfg,
        const std::map<uint32_t, std::filesystem::path>& successful_builds,
        const std::map<uint32_t, std::filesystem::path>& broken_builds,
        const std::map<uint32_t, std::filesystem::path>& pending_builds,
        cis1::os_interface& os)
    : name_(name)
    , config_(cfg)
    , successful_builds_(successful_builds)
    , broken_builds_(broken_builds)
    , pending_builds_(pending_builds)
    , os_(os)
{}

void job::cleanup(std::error_code& ec)
{
    size_t successful_builds_to_erase =
            (successful_builds_.size() > config_.keep_successful_builds
            ? successful_builds_.size() - config_.keep_successful_builds
            : 0);

    for(size_t i = 0; i < successful_builds_to_erase; ++i)
    {
        os_.remove_all(successful_builds_.begin()->second, ec);

        if(ec)
        {
            return;
        }

        successful_builds_.erase(successful_builds_.begin());
    }

    size_t broken_builds_to_erase =
            (broken_builds_.size() > config_.keep_broken_builds
            ? broken_builds_.size() - config_.keep_broken_builds
            : 0);

    for(size_t i = 0; i < broken_builds_to_erase; ++i)
    {
        os_.remove_all(broken_builds_.begin()->second, ec);

        if(ec)
        {
            return;
        }

        broken_builds_.erase(broken_builds_.begin());
    }
}

const std::vector<std::pair<std::string, std::string>>& job::params() const
{
    return config_.params;
}

void job::execute(
        uint32_t build_number,
        cis1::context_interface& ctx,
        std::error_code& ec,
        int& exit_code,
        job_runner_factory_t job_runner_factory)
{
    boost::asio::io_context io_ctx;

    auto& build_dir = pending_builds_[build_number];

    ctx.set_env("build", build_dir.filename());

    auto runner = job_runner_factory(
            io_ctx,
            ctx.env(),
            build_dir,
            os_);

    auto output = os_.open_ofstream(build_dir / "output.txt");
    if(!output || !output->is_open())
    {
        ec = cis1::error_code::cant_open_build_output_file;

        return;
    }

    runner->run(
            config_.script,
            [&](std::error_code err, int exit)
            {
                if(err)
                {
                    ec = err;
                }
                else
                {
                    exit_code = exit;

                    auto ec_file = os_.open_ofstream(build_dir / "exitcode.txt");
                    if(!ec_file || !ec_file->is_open())
                    {
                        ec = cis1::error_code::cant_open_build_exit_code_file;

                        return;
                    }

                    ec_file->ostream() << exit << std::endl;
                }
            },
            [&](const std::string& str)
            {
                output->ostream() << str << '\n';
            },
            [&](const std::string& str)
            {
                output->ostream() << str << '\n';
            });

    io_ctx.run();

    if(ec
        && ec != cis1::error_code::cant_open_build_exit_code_file)
    {
        ec = cis1::error_code::cant_execute_script;
    }

    auto node = pending_builds_.extract(build_number);

    if(exit_code == 0)
    {
        successful_builds_.insert(std::move(node));
    }
    else
    {
        broken_builds_.insert(std::move(node));
    }
}

job::build_handle job::prepare_build(
        const cis1::context_interface& ctx,
        const session_interface& session,
        const std::vector<std::pair<std::string, std::string>>& params,
        std::error_code& ec)
{
    int max_build_num = -1;

    for(auto& entry : os_.list_directory(ctx.base_dir() / "jobs" / name_))
    {
        if(entry->is_directory() && is_build(entry->path().filename().string()))
        {
            int build_num = std::stoi(entry->path().filename().string());
            if(build_num > max_build_num)
            {
                max_build_num = build_num;
            }
        }
    }

    ++max_build_num;

    //TODO handle build_num > 999999

    std::stringstream ss;

    ss << std::setfill('0') << std::setw(6) << max_build_num;

    auto build_dir = ctx.base_dir() / "jobs" / name_ / ss.str();

    os_.create_directory(build_dir, ec);
    if(ec)
    {
        return {*this, std::nullopt};
    }

    os_.copy(
            ctx.base_dir() / "jobs" / name_ / config_.script,
            build_dir / config_.script,
            ec);
    if(ec)
    {
        return {*this, std::nullopt};
    }

    auto os = os_.open_ofstream(build_dir / "job.params");
    if(!os || !os->is_open())
    {
        ec = cis1::error_code::cant_write_job_params_file;

        return {*this, std::nullopt};
    }

    for(auto& [k, v] : params)
    {
        os->ostream() << k << "=" << v << "\n";
    }

    os_.copy(ctx.base_dir() / "jobs" / name_ / "job.conf", build_dir / "job.conf", ec);
    if(ec)
    {
        ec = cis1::error_code::cant_write_job_conf_file;

        return {*this, std::nullopt};
    }

    os = os_.open_ofstream(build_dir / "session_id.txt");
    if(!os || !os->is_open())
    {
        ec = cis1::error_code::cant_write_session_id_file;

        return {*this, std::nullopt};
    }

    os->ostream() << session.session_id() << std::endl;

    pending_builds_.emplace(max_build_num, build_dir);

    return {*this, max_build_num};
}

std::optional<job> load_job(
        const std::string& job_name,
        std::error_code& ec,
        cis1::context_interface& ctx,
        cis1::os_interface& os)
{
    auto job_path = ctx.base_dir() / "jobs" / job_name;

    if(!os.exists(job_path, ec) || ec)
    {
        ec = error_code::job_dir_doesnt_exist;

        return std::nullopt;
    }

    auto job_conf = os.open_ifstream(job_path / "job.conf");

    if(!job_conf || !job_conf->is_open())
    {
        ec = error_code::cant_read_job_conf_file;

        return std::nullopt;
    }

    std::map<std::string, std::string> conf;

    cis1::read_istream_kv_str(job_conf->istream(), conf, ec);

    if(ec
    || conf.count("script") != 1
    || conf.count("keep_last_success_builds") != 1
    || conf.count("keep_last_success_builds") != 1)
    {
        ec = error_code::cant_read_job_conf_file;

        return std::nullopt;
    }

    auto keep_successful_builds = u32_from_string(conf["keep_last_success_builds"]);

    auto keep_broken_builds = u32_from_string(conf["keep_last_break_builds"]);

    if(!keep_successful_builds || !keep_broken_builds)
    {
        ec = error_code::cant_read_job_conf_file;

        return std::nullopt;
    }

    if(!os.exists(job_path / conf["script"], ec) || ec)
    {
        ec = error_code::script_doesnt_exist;

        return std::nullopt;
    }

    auto is = os.open_ifstream(job_path / "job.params");
    std::vector<std::pair<std::string, std::string>> job_params;
    if(is && is->is_open())
    {
        read_istream_ordered_kv_str(
                is->istream(),
                job_params,
                ec);

        if(ec)
        {
            ec = cis1::error_code::cant_read_job_params_file;

            return std::nullopt;
        }
    }

    std::map<uint32_t, std::filesystem::path> successful_builds;
    std::map<uint32_t, std::filesystem::path> broken_builds;
    std::map<uint32_t, std::filesystem::path> pending_builds;

    for(auto& entry_ptr : os.list_directory(job_path))
    {
        auto& entry = *entry_ptr;

        if(is_build(entry.path().filename().string()))
        {
            auto exitcode = [&]() -> std::optional<uint32_t>
            {
                auto exitcode_file = os.open_ifstream(entry.path() / "exitcode.txt");

                if(!exitcode_file)
                {
                    return std::nullopt;
                }

                std::string exitcode_str;

                std::getline(exitcode_file->istream(), exitcode_str);

                return u32_from_string(exitcode_str);
            }();

            auto build_num = stoul(entry.path().filename().string());

            if(!exitcode)
            {
                pending_builds.emplace(build_num, entry.path());

                continue;
            }

            if(*exitcode == 0)
            {
                successful_builds.emplace(build_num, entry.path());
            }
            else
            {
                broken_builds.emplace(build_num, entry.path());
            }
        }
    }

    return job{
        job_name,
        job::config{
            conf["script"],
            keep_successful_builds.value(),
            keep_broken_builds.value(),
            job_params
        },
        successful_builds,
        broken_builds,
        pending_builds,
        os};
}

void prepare_params(
        std::vector<std::pair<std::string, std::string>>& params,
        const os_interface& os,
        const context_interface& ctx,
        const session_interface& session,
        std::error_code& ec)
{
    auto session_prm = ctx.base_dir()
            / "sessions" / (session.session_id() + ".prm");
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

    for(auto& [k, v] : params)
    {
        if(auto it = values.find(k); it != values.end())
        {
            v = it->second;
        }
    }
}

} // namespace cis1
