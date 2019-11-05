#pragma once

#include <set>
#include <fstream>
#include <filesystem>

#include <boost/asio.hpp>

#include <croncpp.h>

#include "context_interface.h"
#include "os_interface.h"

class cron_entry
{
public:
    cron_entry(
            const std::string& job,
            const std::string& expr);

    bool operator<(const cron_entry& other) const;

    const std::string& job() const;

    const std::string& expr() const;

private:
    std::string job_;
    std::string expr_;
};

class cron_list
{
public:
    cron_list(
            const std::filesystem::path& path,
            const std::set<cron_entry>& crons,
            cis1::os_interface& os);

    void save();

    const std::set<cron_entry>& list() const;

    void add(const cron_entry& expr);

    size_t del(const cron_entry& expr);

private:
    std::filesystem::path crons_file_path_;
    std::set<cron_entry> crons_;
    cis1::os_interface& os_;
};

std::optional<cron_list> load_cron_list(
        const std::filesystem::path& path,
        std::error_code& ec,
        cis1::os_interface& os);

struct cron_timer
{
    cron_timer(
            const cron::cronexpr& expr,
            boost::asio::io_context& ctx);

    void run(const std::function<void()>& cb);

    void cancel();

private:
    cron::cronexpr expr_;
    boost::asio::steady_timer timer_;
};

class cron_manager
{
public:
    cron_manager(
            boost::asio::io_context& io_ctx,
            cis1::context_interface& ctx,
            const std::filesystem::path& path,
            cis1::os_interface& os);

    void update();

private:
    boost::asio::io_context& io_ctx_;
    cis1::context_interface& ctx_;
    std::filesystem::path crons_file_path_;
    cis1::os_interface& os_;
    std::map<cron_entry, cron_timer> timers_;

    void run_job(const std::string& job);
};
