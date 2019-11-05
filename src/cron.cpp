#include "cron.h"

#include <boost/process/spawn.hpp>

#include "logger.h"

cron_entry::cron_entry(
        const std::string& job,
        const std::string& expr)
    : job_(job)
    , expr_(expr)
{}

bool cron_entry::operator<(const cron_entry& other) const
{
    return std::tie(job_, expr_)
         < std::tie(other.job_, other.expr_);
}

const std::string& cron_entry::job() const
{
    return job_;
}

const std::string& cron_entry::expr() const
{
    return expr_;
}

cron_list::cron_list(
        const std::filesystem::path& path,
        const std::set<cron_entry>& crons,
        cis1::os_interface& os)
    : crons_file_path_(path)
    , crons_(crons)
    , os_(os)
{}

void cron_list::save()
{
    auto crons_file = os_.open_ofstream(crons_file_path_);

    for(auto& cron : crons_)
    {
        crons_file->ostream() << cron.job() << " "
                   << cron.expr() << std::endl;
    }
}

const std::set<cron_entry>& cron_list::list() const
{
    return crons_;
}

void cron_list::add(const cron_entry& expr)
{
    crons_.insert(expr);
}

size_t cron_list::del(const cron_entry& expr)
{
    return crons_.erase(expr);
}

std::optional<cron_list> load_cron_list(
        const std::filesystem::path& path,
        std::error_code& ec,
        cis1::os_interface& os)
{
    auto crons_file = os.open_ifstream(path);

    if(!crons_file->is_open())
    {
        return std::nullopt;
    }

    std::set<cron_entry> crons;

    while(crons_file->istream().good())
    {
        std::string job;
        std::string cron;

        std::getline(crons_file->istream(), job, ' ');
        std::getline(crons_file->istream(), cron, '\n');

        if(job.empty() || cron.empty())
        {
            break;
        }

        crons.insert(cron_entry{job, cron});
    }

    return cron_list{path, crons, os};
}

cron_timer::cron_timer(
        const cron::cronexpr& expr,
        boost::asio::io_context& ctx)
    : expr_(expr)
    , timer_(ctx)
{}

void cron_timer::run(const std::function<void()>& cb)
{
    auto now = std::time(0);
    auto next = cron::cron_next(expr_, now);
    auto time_to_next = std::chrono::system_clock::from_time_t(next)
                      - std::chrono::system_clock::from_time_t(now);

    timer_.expires_after(time_to_next);

    timer_.async_wait(
        [&, cb](const boost::system::error_code& error)
        {
            if(!error)
            {
                cb();
                run(cb);
            }
        });
}

void cron_timer::cancel()
{
    timer_.cancel();
}

cron_manager::cron_manager(
        boost::asio::io_context& io_ctx,
        cis1::context_interface& ctx,
        const std::filesystem::path& path,
        cis1::os_interface& os)
    : io_ctx_(io_ctx)
    , ctx_(ctx)
    , crons_file_path_(path)
    , os_(os)
{}

void cron_manager::update()
{
    auto crons_file = os_.open_ifstream(crons_file_path_);

    if(!crons_file->is_open())
    {
        return;
    }

    std::set<cron_entry> crons;

    while(crons_file->istream().good())
    {
        std::string job;
        std::string cron;

        std::getline(crons_file->istream(), job, ' ');
        std::getline(crons_file->istream(), cron, '\n');

        if(job.empty() || cron.empty())
        {
            break;
        }

        crons.insert(cron_entry{job, cron});
    }

    auto new_it = crons.begin();
    auto new_end = crons.end();
    auto old_it = timers_.begin();
    auto old_end = timers_.end();

    while(new_it != new_end && old_it != old_end)
    {
        if(*new_it < old_it->first)
        {
            auto cronexpr = cron::make_cron(new_it->expr());
            auto it = timers_.emplace_hint(
                    old_it,
                    *new_it,
                    cron_timer{cronexpr, io_ctx_});

            it->second.run(
                    [&, job = it->first.job()]()
                    {
                        run_job(job);
                    });

            ++new_it;
        }
        else if(old_it->first < *new_it)
        {
            old_it->second.cancel();
            old_it = timers_.erase(old_it);
        }
        else
        {
            ++old_it;
            ++new_it;
        }
    }

    for(; old_it != old_end; old_it = timers_.erase(old_it))
    {
        old_it->second.cancel();
    }

    for(; new_it != new_end; ++new_it)
    {
        auto cronexpr = cron::make_cron(new_it->expr());
        auto it = timers_.emplace_hint(
                old_it,
                *new_it,
                cron_timer{cronexpr, io_ctx_});

        it->second.run(
                [&, job = it->first.job()]()
                {
                    run_job(job);
                });
    }
}

void cron_manager::run_job(const std::string& job)
{
    try
    {
        auto executable =
                std::filesystem::path{"core"}
                / ctx_.env().at("startjob").to_vector()[0];

        os_.spawn_process(
                ctx_.base_dir().generic_string(),
                executable.generic_string(),
                std::vector<std::string>{job},
                ctx_.env());
    }
    catch(boost::process::process_error ex)
    {
        cis_log() << "action=\"error\" "
                  << "Cron can't start job " << job << std::endl;
    }
}
