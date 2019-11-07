#include <iostream>

#include "context.h"
#include "os.h"
#include "logger.h"
#include "read_istream_kv_str.h"
#include "utils.h"
#include "maintenance.h"

void usage(const char* self_name)
{
    std::cout << "Usage: \n"
              << "\t" << self_name << " --job ${job_name}" << std::endl;
}

int clean_job(
        const std::string& job_name,
        cis1::context_interface& ctx,
        cis1::os_interface& os)
{
    std::error_code ec;

    auto job_path = ctx.base_dir() / "jobs" / job_name;

    if(!os.exists(job_path, ec) || ec)
    {
        std::cout << "Job doesn't exist" << std::endl;

        return EXIT_FAILURE;
    }

    auto job_conf = os.open_ifstream(job_path / "job.conf");

    if(!job_conf || !job_conf->is_open())
    {
        std::cout << "Can't read job conf" << std::endl;

        return EXIT_FAILURE;
    }

    std::map<std::string, std::string> conf;

    cis1::read_istream_kv_str(job_conf->istream(), conf, ec);

    if(ec
    || conf.count("keep_last_success_builds") != 1
    || conf.count("keep_last_success_builds") != 1)
    {
        std::cout << "Can't read job conf" << std::endl;

        return EXIT_FAILURE;
    }

    auto keep_succ_builds = u32_from_string(conf["keep_last_success_builds"]);

    auto keep_break_builds = u32_from_string(conf["keep_last_break_builds"]);

    if(!keep_succ_builds || !keep_break_builds)
    {
        std::cout << "Can't read job conf" << std::endl;

        return EXIT_FAILURE;
    }

    std::map<uint32_t, std::filesystem::path> successful_builds;
    std::map<uint32_t, std::filesystem::path> broken_builds;

    for(auto it = std::filesystem::directory_iterator(job_path);
             it != std::filesystem::directory_iterator();
             it.increment(ec))
    {
        if(ec)
        {
            std::cout << "Filesystem error" << std::endl;

            return EXIT_FAILURE;
        }
        auto& entry = *it;

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

            if(!exitcode)
            {
                continue;
            }

            auto build_num = stoul(entry.path().filename().string());

            if(*exitcode != 0)
            {
                successful_builds.emplace(build_num, entry.path());
            }
            else
            {
                broken_builds.emplace(build_num, entry.path());
            }
        }
    }

    size_t succ_builds_to_erase = (successful_builds.size() > *keep_succ_builds ?
                  successful_builds.size() - *keep_succ_builds : 0);

    for(size_t i = 0; i < succ_builds_to_erase; ++i)
    {
        os.remove_all(successful_builds.begin()->second, ec);
        successful_builds.erase(successful_builds.begin());
    }

    size_t break_builds_to_erase = (broken_builds.size() > *keep_break_builds ?
                  broken_builds.size() - *keep_break_builds : 0);

    for(size_t i = 0; i < break_builds_to_erase; ++i)
    {
        os.remove_all(broken_builds.begin()->second, ec);
        broken_builds.erase(broken_builds.begin());
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    cis1::os std_os;

    std::error_code ec;

    auto ctx_opt = cis1::init_context(ec, std_os);
    if(ec)
    {
        std::cerr << ec.message() << std::endl;

        return 1;
    }
    auto& ctx = ctx_opt.value();

    init_cis_log(ctx);

    if(argc != 3 || strcmp(argv[1], "--job") != 0)
    {
        usage(argv[0]);

        return EXIT_FAILURE;
    }

    return clean_job(argv[2], ctx, std_os);
}
