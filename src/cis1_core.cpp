/*

    Class cis1_core members implementation

*/

#include "cis1_core.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <regex>

#include <boost/process/environment.hpp>

#include "cis1_error_code.h"
#include "job_runner.h"

cis1_core::cis1_core()
    : env_(boost::this_process::environment())
{}

cis1_core::~cis1_core()
{
    // TODO closesession to sessionlog/corelog ???
}

void cis1_core::init(std::error_code& ec)
{
    if(status_ != status::not_init)
    {
        ec = cis1::error::error_code::bad_state;
        return;
    }

    cis_base_dir_ = get_env_var("cis_base_dir");

    if(cis_base_dir_.empty())
    {
        ec = cis1::error::error_code::base_dir_not_defined;

        status_ = status::error;

        return;
    }

    if(!std::filesystem::is_directory(cis_base_dir_, ec) || ec)
    {
        ec = cis1::error::error_code::base_dir_doesent_exist;

        status_ = status::error;

        return;
    }

    std::map<std::string, std::string> executables;
    read_file_str(
            cis_base_dir_ / "core" / "cis.conf",
            executables);

    for(auto& [key, value] : executables)
    {
        env_[key] = value;
    }

    // TODO: init corelog system

    invoke_session(ec);
    if(ec)
    {
        status_ = status::error;

        // TODO: corelog

        return;
    }

    // TODO: corelog

    // TODO: init sessionlog, log about new sessoin if need

    status_ = status::ok;
}

cis1_core::status cis1_core::get_status()
{
    return status_;
}

bool cis1_core::session_opened_by_me()
{
    return session_opened_by_me_;
};

std::string cis1_core::get_session_id()
{
    return session_id_;
}

/*! \fn invoke_session
    \brief Connect to exist session or create new if need
    @return void
*/
void cis1_core::invoke_session(std::error_code& ec)
{
    if(status_ == status::error)
    {
        ec = cis1::error::error_code::bad_state;

        return;
    }

    session_id_ = get_env_var("session_id");

    if(!session_id_.empty())
    {
        session_opened_by_me_ = false;

        env_["session_id"] = session_id_;
        env_["session_opened_by_me"] = "false";

        return;
    }

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto id = boost::this_process::get_id();
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d-%H-%M-%S-") << id;
    session_id_ = ss.str();

    env_["session_id"] = session_id_;
    env_["session_opened_by_me"] = "true";

    session_opened_by_me_ = true;

    return;
}

/*! \fn startjob
    \brief starting job
    @return void
*/
void cis1_core::startjob(
        const std::string& job_name,
        int& exit_code,
        std::error_code& ec)
{
    if(status_ != status::ok)
    {
        ec = cis1::error::error_code::bad_state;

        return;
    }

    env_["job_name"] = job_name;

    auto job_dir = cis_base_dir_ / "jobs" / job_name;

    // TODO: check jobname env consistent (dir, config file, script)

    std::map<std::string, std::string> job_conf;
    if(!read_file_str(job_dir / "job.conf", job_conf)
            || job_conf.find("script") == job_conf.end())
    {
        ec = cis1::error::error_code::cant_read_job_conf_file;

        // TODO: corelog, session log

        return;
    }

    auto script_file_name = job_conf["script"];

    if(!std::filesystem::exists(job_dir / script_file_name))
    {
        ec = cis1::error::error_code::script_doesnt_exist;

        // TODO: corelog, session log

        return;
    }

    std::map<std::string, std::string> job_params;
    if(!read_file_str(job_dir / "job.params", job_params))
    {
        ec = cis1::error::error_code::cant_read_job_params_file;

        // TODO: corelog, session log

        return;
    }

    if(session_opened_by_me_)
    {
        read_job_params(job_params);
    }

    auto build_num = get_new_build_dir(job_dir);

    if(build_num.empty())
    {
        ec = cis1::error::error_code::cant_generate_build_num;

        // TODO: corelog, session log

        return;
    }

    env_["build_number"] = build_num;

    if(!std::filesystem::create_directory(job_dir / build_num, ec) || ec)
    {
        ec = cis1::error::error_code::cant_create_build_dir;

        // TODO corelog, session log

        return;
    }

    std::filesystem::copy(
                job_dir / script_file_name,
                job_dir / build_num / script_file_name,
                ec);

    if(ec)
    {
        ec = cis1::error::error_code::cant_copy_script;

        // TODO corelog, session log

        return;
    }

    write_job_params(
            job_dir / build_num / "job.params",
            job_params,
            ec);
    if(ec)
    {
        return;
    }

    exit_code = execute_job(
            job_dir / build_num,
            script_file_name,
            ec);
    if(ec)
    {
        return;
    }


    setvalue("last_job_name", job_name, ec);
    if(ec)
    {
        return;
    }

    setvalue("last_job_build_number", build_num, ec);
    if(ec)
    {
        return;
    }

    // TODO: corelog, session log
}

void cis1_core::setparam(
        const std::string& param_name,
        const std::string& param_value,
        std::error_code& ec)
{
    if(status_ != status::ok)
    {
        ec = cis1::error::error_code::bad_state;

        return;
    }

    auto session_prm = cis_base_dir_ / "sessions" / (session_id_ + ".prm");

    std::map<std::string, std::string> values;
    if(!read_file_str(session_prm, values))
    {
        ec = cis1::error::error_code::cant_read_session_params_file;

        // TODO: corelog, session log

        return;
    }

    std::ofstream session_prm_file(session_prm, std::ios::trunc);
    bool found = false;
    for(auto& [k, v] : values)
    {
        if(k == param_name)
        {
            v = param_value;
            found = true;
        }

        session_prm_file << k << '=' << v << '\n';
    }
    if(!found)
    {
        session_prm_file << param_name << '=' << param_value << '\n';
    }

    // TODO check status, check session

    // TODO sheck if param exist in job.param

    // TODO read session prm file and replace the param value or add it to the prm file
}

void cis1_core::getparam(
        const std::string& param_name,
        std::string& param_value,
        std::error_code& ec)
{
    if(status_ != status::ok)
    {
        ec = cis1::error::error_code::bad_state;

        return;
    }

    auto session_prm = cis_base_dir_ / "jobs"
                        / get_env_var("job_name") / get_env_var("build_number")
                        / "job.params";

    std::map<std::string, std::string> values;
    if(!read_file_str(session_prm, values))
    {
        ec = cis1::error::error_code::cant_read_job_params_file;

        // TODO: corelog, session log

        return;
    }


    if(auto it = values.find(param_name); it != values.end())
    {
        param_value.assign(it->second);
    }

    // TODO check status, check session

    // TODO check if param exist in job.param

    // TODO read session prm file and find and return value

    // TODO ERRROR if param not found in session prm file
}

void cis1_core::setvalue(
        const std::string& value_name,
        const std::string& value,
        std::error_code& ec)
{
    if(status_ != status::ok)
    {
        ec = cis1::error::error_code::bad_state;

        return;
    }

    auto session_dat = cis_base_dir_ / "sessions" / (session_id_ + ".dat");

    std::map<std::string, std::string> values;
    if(!read_file_str(session_dat, values))
    {
        ec = cis1::error::error_code::cant_read_session_values_file;

        // TODO: corelog, session log

        return;
    }

    std::ofstream session_dat_file(session_dat, std::ios::trunc);
    bool found = false;
    for(auto& [k, v] : values)
    {
        if(k == value_name)
        {
            v = value;
            found = true;
        }
        session_dat_file << k << '=' << v << '\n';
    }

    if(!found)
    {
        session_dat_file << value_name << '=' << value << '\n';
    }

    // TODO check if value exist in job.value

    // TODO read session prm file and replace the value value or add it to the prm file
}

void cis1_core::getvalue(
        const std::string& value_name,
        std::string& value,
        std::error_code& ec)
{
    if(status_ != status::ok)
    {
        ec = cis1::error::error_code::bad_state;

        return;
    }

    auto session_dat = cis_base_dir_ / "sessions" / (session_id_ + ".dat");

    std::map<std::string, std::string> values;
    if(!read_file_str(session_dat, values))
    {
        ec = cis1::error::error_code::cant_read_session_values_file;

        // TODO: corelog, session log

        return;
    }


    if(auto it = values.find(value_name); it != values.end())
    {
        value.assign(it->second);
    }


    // TODO check if value exist in job.value

    // TODO read session prm file and find and return value

    // TODO ERRROR if value not found in session prm file
}

std::string cis1_core::get_env_var(const std::string& var_name)
{
    auto env = boost::this_process::environment();

    auto it = env.find(var_name);

    if(it != env.end())
    {
        return it->to_vector()[0];
    }

    return {};
}

std::string cis1_core::get_new_build_dir(const std::filesystem::path& dir)
{
    static auto is_build = [](const std::string& dir_name)
    {
        static const std::regex build_mask("^\\d{6}$");
        return std::regex_match(dir_name, build_mask);
    };

    int max_build_num = -1;

    for(auto& entry : std::filesystem::directory_iterator(dir))
    {
        if(entry.is_directory() && is_build(entry.path().filename().string()))
        {
            int build_num = std::stoi(entry.path().filename().string());
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

    return ss.str();
}

void cis1_core::read_job_params(std::map<std::string, std::string>& params)
{
    for(auto& [k, v] : params)
    {
        std::cout << "Type param value for parameter " << k
                  << "(\"Enter\" for default value:\""
                  << v << "\")" << std::endl;

        std::string tmp;
        std::getline(std::cin, tmp, '\n');

        if(!tmp.empty())
        {
            v = tmp;
        }
    }
}

void cis1_core::write_job_params(
        const std::filesystem::path& path,
        const std::map<std::string, std::string>& job_params,
        std::error_code& ec)
{
    std::ofstream job_params_file(path);

    if(session_opened_by_me_)
    {
        for(auto& [k, v] : job_params)
        {
            job_params_file << k << '=' << v << '\n';
        }
    }
    else
    {
        auto session_prm = cis_base_dir_ / "sessions" / (session_id_ + ".prm");
        std::map<std::string, std::string> session_prm_values;
        if(!read_file_str(session_prm, session_prm_values))
        {
            ec = cis1::error::error_code::cant_read_session_params_file;

            // TODO: corelog, session log

            return;
        }

        for(auto& [k, v] : job_params)
        {
            job_params_file << k << '=';

            if(auto it = session_prm_values.find(k); it != session_prm_values.end())
            {
                job_params_file << it->second;
            }
            else
            {
                job_params_file << v;
            }

            job_params_file << '\n';
        }
    }
}

int cis1_core::execute_job(
        const std::filesystem::path& path,
        const std::string& script,
        std::error_code& ec)
{
    boost::asio::io_context ctx;

    job_runner runner(
            ctx,
            env_,
            path);

    std::ofstream output(path / "output.txt");

    int exit_code = -1;

    runner.run(
            script,
            [&](std::error_code err, int exit)
            {
                if(err)
                {
                    ec = err;
                    exit_code = -1;
                }
                else
                {
                    std::ofstream file(path / "exit_code.txt");
                    file << exit;
                    exit_code = exit;
                }
            },
            [&](const std::string& str)
            {
                output << str;
            });

    ctx.run();

    if(ec)
    {
        ec = cis1::error::error_code::cant_execute_script;
    }

    return exit_code;
}

bool cis1_core::read_file_str(
        const std::filesystem::path& path,
        std::map<std::string, std::string>& result)
{
    std::ifstream file(path);

    while(file.good())
    {
        std::string key;
        std::string val;
        std::getline(file, key, '=');
        std::getline(file, val, '\n');

        val.erase(
                std::find_if(
                        val.rbegin(),
                        val.rend(),
                        [](int ch)
                        {
                            return !std::isspace(ch);
                        }).base(),
                val.end());

        if(!key.empty())
        {
            result[key] = val;
        }
    }

    return true;
}
