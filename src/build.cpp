#include "build.h"

#include <regex>
#include <sstream>
#include <iomanip>

#include <boost/asio.hpp>

#include "job_runner.h"
#include "error_code.h"
#include "read_istream_kv_str.h"

namespace cis1
{

build::build(
        const std::string& job_name,
        const std::filesystem::path& job_dir,
        const std::filesystem::path& script,
        const std::map<std::string, std::string>& params,
        const os_interface& os)
    : job_name_(job_name)
    , job_dir_(job_dir)
    , script_(script)
    , params_(params)
    , os_(os)
{}

std::map<std::string, std::string>& build::params()
{
    return params_;
}

void build::create_build_dir(std::error_code& ec)
{
    static auto is_build = [](const std::string& dir_name)
    {
        static const std::regex build_mask("^\\d{6}$");
        return std::regex_match(dir_name, build_mask);
    };

    int max_build_num = -1;

    for(auto& entry : os_.list_directory(job_dir_))
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

    build_dir_ = job_dir_ / ss.str();

    os_.create_directory(build_dir_, ec);
}

void build::prepare_params(
        const context_interface& ctx,
        const session_interface& session,
        std::error_code& ec)
{
    auto session_prm = ctx.base_dir() / "sessions" / (session.session_id() + ".prm");
    std::map<std::string, std::string> values;
    if(os_.exists(session_prm, ec))
    {
        auto session_prm_file = os_.open_ifstream(session_prm);
        if(!session_prm_file || !session_prm_file->is_open())
        {
            ec = cis1::error_code::cant_read_session_values_file;

            // TODO: corelog, session log

            return;
        }
        read_istream_kv_str(session_prm_file->istream(), values, ec);
        if(ec)
        {
            ec = cis1::error_code::cant_read_session_values_file;

            // TODO: corelog, session log

            return;
        }
    }

    for(auto it1 = params_.begin(), it2 = values.begin(); it1 != params_.end(); ++it1)
    {
        while(it2 != values.end() && it1->first > it2->first)
        {
            ++it2;
        }

        if(it2 == values.end())
        {
            break;
        }
        else if(it1->first == it2->first)
        {
            it1->second = it2->second;
        }
    }
}

void build::prepare_build_dir(std::error_code& ec)
{
    create_build_dir(ec);
    if(ec)
    {
        return;
    }

    os_.copy(script_, build_dir_ / script_.filename(), ec);
    if(ec)
    {
        return;
    }

    auto os = os_.open_ofstream(build_dir_ / "job.params");
    if(!os || !os->is_open())
    {
        ec = cis1::error_code::cant_write_job_params_file;

        return;
    }

    for(auto& [k, v] : params_)
    {
        os->ostream() << k << "=" << v << "\n";
    }

    os_.copy(job_dir_ / "job.conf", build_dir_ / "job.conf", ec);
    if(ec)
    {
        ec = cis1::error_code::cant_write_job_conf_file;

        return;
    }
}

void build::execute(
        context_interface& ctx,
        std::error_code& ec,
        int& exit_code,
        job_runner_factory_t job_runner_factory)
{
    boost::asio::io_context io_ctx;

    ctx.set_env("build", build_dir_.filename());

    auto runner = job_runner_factory(
            io_ctx,
            ctx.env(),
            build_dir_,
            os_);

    auto output = os_.open_ofstream(build_dir_ / "output.txt");
    if(!output || !output->is_open())
    {
        ec = cis1::error_code::cant_open_build_output_file;

        return;
    }

    runner->run(
            script_.filename(),
            [&](std::error_code err, int exit)
            {
                if(err)
                {
                    ec = err;
                }
                else
                {
                    exit_code = exit;

                    auto ec_file = os_.open_ofstream(build_dir_ / "exit_code.txt");
                    if(!ec_file || !ec_file->is_open())
                    {
                        ec = cis1::error_code::cant_open_build_exit_code_file;

                        return;
                    }

                    ec_file->ostream() << exit;
                }
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
}

std::string build::build_num()
{
    return build_dir_.filename();
}

std::optional<build> prepare_build(
        context_interface& ctx,
        std::string job_name,
        std::error_code& ec,
        const os_interface& os)
{
    ctx.set_env("job_name", job_name);

    auto job_dir = ctx.base_dir() / "jobs" / job_name;

    if(!os.is_directory(job_dir, ec) || ec)
    {
        ec = cis1::error_code::job_dir_doesnt_exist;

        return std::nullopt;
    }

    auto is = os.open_ifstream(job_dir / "job.conf");
    if(!is || !is->is_open())
    {
        ec = cis1::error_code::cant_read_job_conf_file;

        return std::nullopt;
    }

    std::map<std::string, std::string> job_conf;
    read_istream_kv_str(is->istream(), job_conf, ec);

    if(ec || job_conf.find("script") == job_conf.end())
    {
        ec = cis1::error_code::cant_read_job_conf_file;

        // TODO: corelog, session log

        return std::nullopt;
    }

    auto script_file_name = job_conf["script"];

    if(!os.exists(job_dir / script_file_name, ec) || ec)
    {
        ec = cis1::error_code::script_doesnt_exist;

        // TODO: corelog, session log

        return std::nullopt;
    }

    is = os.open_ifstream(job_dir / "job.params");
    std::map<std::string, std::string> job_params;
    if(is && is->is_open())
    {
        read_istream_kv_str(is->istream(), job_params, ec);

        if(ec)
        {
            ec = cis1::error_code::cant_read_job_params_file;

            // TODO: corelog, session log

            return std::nullopt;
        }
    }

    return build{
            job_name,
            job_dir,
            job_dir / script_file_name,
            job_params,
            os};
}

} // namespace cis1
