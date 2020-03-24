/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include <iostream>

#include "context.h"
#include "os.h"
#include "logger.h"
#include "job.h"
#include "cis_version.h"

void usage(const char* self_name)
{
    std::cout << "Usage: \n"
              << "\t" << self_name << " --job ${job_name}" << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc == 2 && strcmp(argv[1], "--version") == 0)
    {
        print_version();

        return EXIT_SUCCESS;
    }

    cis1::os std_os;

    std::error_code ec;

    auto ctx_opt = cis1::init_context(ec, std_os);
    if(ec)
    {
        std::cerr << ec.message() << std::endl;

        return 1;
    }
    auto& ctx = ctx_opt.value();

    // session id is not specified here
    const auto session_id = std::nullopt;
    const CoreLogger::Options options
            = make_logger_options(session_id, ctx, std_os);

    init_cis_log(options, ctx);

    if(argc != 3 || strcmp(argv[1], "--job") != 0)
    {
        usage(argv[0]);

        return EXIT_FAILURE;
    }

    auto job_opt = cis1::load_job(argv[2], ec, ctx, std_os);
    if(ec)
    {
        std::cout << ec.message() << std::endl;

        return EXIT_FAILURE;
    }
    auto& job = job_opt.value();

    job.cleanup(ec);
    if(ec)
    {
        std::cout << ec.message() << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
