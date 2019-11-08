#include <iostream>

#include "context.h"
#include "os.h"
#include "logger.h"
#include "maintenance.h"

void usage(const char* self_name)
{
    std::cout << "Usage: \n"
              << "\t" << self_name << " --job ${job_name}" << std::endl;
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
