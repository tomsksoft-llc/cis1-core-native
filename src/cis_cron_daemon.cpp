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

int main(int argc, char* argv[])
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

    boost::asio::io_context io_ctx;

    cron_manager cm(io_ctx, ctx, ctx.base_dir() / "core" / "crons", std_os);

    boost::asio::signal_set signals(io_ctx, SIGINT, SIGTERM);

    std::thread update_watcher(
            [&,
            guard = boost::asio::make_work_guard(io_ctx)]() mutable
            {
                boost::interprocess::named_condition cv1(
                        boost::interprocess::open_or_create,
                        "cis1_cron_cv1");

                signals.async_wait(
                        [&](const boost::system::error_code& ec, int)
                        {
                            io_ctx.stop();
                            cv1.notify_one();
                        });

                boost::interprocess::named_mutex m1(
                        boost::interprocess::open_or_create,
                        "cis1_cron_m1");

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
