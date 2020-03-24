/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include <thread>
#include <set>
#include <chrono>
#include <string>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <croncpp.h>

#include "context.h"
#include "logger.h"
#include "os.h"
#include "cron.h"
#include "cis_version.h"

int main(int argc, char* argv[])
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

    boost::asio::io_context io_ctx;

    cron_manager cm(io_ctx, ctx, ctx.base_dir() / "core" / "crons", std_os);

    boost::asio::signal_set signals(io_ctx, SIGINT, SIGTERM);

#ifdef __linux__
    std::filesystem::remove("/dev/shm/cis1_cron_cv1", ec);
    if(ec)
    {
        std::cout << ec.message() << std::endl;

        return EXIT_FAILURE;
    }

    std::filesystem::remove("/dev/shm/sem.cis1_cron_m1", ec);
    if(ec)
    {
        std::cout << ec.message() << std::endl;

        return EXIT_FAILURE;
    }
#endif

    boost::interprocess::named_condition cv1(
            boost::interprocess::create_only,
            "cis1_cron_cv1");

    boost::interprocess::named_mutex m1(
            boost::interprocess::create_only,
            "cis1_cron_m1");

    signals.async_wait(
            [&](const boost::system::error_code& ec, int)
            {
                io_ctx.stop();
                cv1.notify_one();
            });

    std::thread update_watcher(
            [&,
            guard = boost::asio::make_work_guard(io_ctx)]() mutable
            {
                boost::interprocess::scoped_lock lock(m1);

                while(!io_ctx.stopped())
                {
                    io_ctx.post(
                            [&]()
                            {
                                cm.update();
                            });

                    cv1.wait(lock);
                }

                guard.reset();
            });

    io_ctx.run();

    update_watcher.join();
}
