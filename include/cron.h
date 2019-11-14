/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <set>
#include <fstream>
#include <filesystem>

#include <boost/asio.hpp>
#include <gtest/gtest_prod.h>
#include <croncpp.h>

#include "context_interface.h"
#include "os_interface.h"

/**
 * \brief Cron entry
 */
class cron_entry
{
public:
    /**
     * \brief Constructs cron_entry instance
     * @param[in] job Job to execute
     * @param[in] expr Cron expression
     */
    cron_entry(
            const std::string& job,
            const std::string& expr);

    /**
     * \brief Compare this entry < other
     */
    bool operator<(const cron_entry& other) const;

    /**
     * \brief Compare this entry == other
     */
    bool operator==(const cron_entry& other) const;

    /**
     * \brief Getter for job member
     * \return Job name
     */
    const std::string& job() const;

    /**
     * \brief Getter for expr member
     * \return Expression string
     */
    const std::string& expr() const;

private:
    std::string job_;
    std::string expr_;
};

/**
 * \brief Cron list representation
 */
class cron_list
{
public:
    /**
     * \brief Constructs cron_list instance
     * @param[in] path Path to crons file
     * @param[in] crons Set with cron entries
     * @param[in] os
     */
    cron_list(
            const std::filesystem::path& path,
            const std::set<cron_entry>& crons,
            cis1::os_interface& os);

    /**
     * \brief Saves cron list to file
     * @param[out] ec
     */
    void save(std::error_code& ec);

    /**
     * \brief Gives access to crons_ member
     */
    const std::set<cron_entry>& list() const;

    /**
     * \brief Add new expression to list
     * @param[in] expr Expression to add
     */
    void add(const cron_entry& expr);

    /**
     * \brief Remove expression from list
     * \return Removed expressions count
     * @param[in] expr Expression to remove
     */
    size_t del(const cron_entry& expr);

private:
    std::filesystem::path crons_file_path_;
    std::set<cron_entry> crons_;
    cis1::os_interface& os_;
};

/**
 * \brief Constructs cron_list if file exists and valid
 * \return cron_list if everything is ok std::nullopt otherwise
 * @param[in] path Path to crons file
 * @param[out] ec
 * @param[in] os
 */
std::optional<cron_list> load_cron_list(
        const std::filesystem::path& path,
        std::error_code& ec,
        cis1::os_interface& os);

/**
 * \brief Timer which manages single cron
 */
class cron_timer
{
public:
    /**
     * \brief Constructs cron_timer instance
     * @param[in] expr Cron expression to manage
     * @param[in] ctx
     */
    cron_timer(
            const cron::cronexpr& expr,
            boost::asio::io_context& ctx);

    /**
     * \brief Starts timer according to cron expr
     * @param[in] cb Callback to execute when timer expires
     */
    void run(const std::function<void()>& cb);

    /**
     * \brief Cancels timer
     */
    void cancel();

private:
    cron::cronexpr expr_;
    boost::asio::steady_timer timer_;
};

/// \cond DO_NOT_DOCUMENT
class cron_manager_run_job_Test;
class cron_manager_update_Test;
/// \endcond

/**
 * \brief CRON expressions manager
 *        Executes jobs automatically.
 */
class cron_manager
{
public:
    /**
     * \brief Constructs cron_manager instance
     * @param[in] io_ctx
     * @param[in] ctx
     * @param[in] path Path to crons file
     * @param[in] os
     */
    cron_manager(
            boost::asio::io_context& io_ctx,
            cis1::context_interface& ctx,
            const std::filesystem::path& path,
            cis1::os_interface& os);
    /**
     * \brief Load all cron entries from file
     */
    void update();

    /// \cond DO_NOT_DOCUMENT
    FRIEND_TEST(::cron_manager, run_job);
    FRIEND_TEST(::cron_manager, update);
    /// \endcond

private:
    boost::asio::io_context& io_ctx_;
    cis1::context_interface& ctx_;
    std::filesystem::path crons_file_path_;
    cis1::os_interface& os_;
    std::map<cron_entry, cron_timer> timers_;

    /**
     * \brief Execute job with given name
     * @param[in] job job name to execute
     */
    void run_job(const std::string& job);
};

/**
 * \brief Notifies daemon about new entries
 *
 *  Internally uses named_condition and named_mutex
 *  thus can block if daemon crashed or in some other
 *  conditions.
 */
void notify_daemon();

/**
 * \brief Starts cis_cron daemon
 * \return EXIT_SUCCES if daemon started successfully EXIT_FAILURE otherwise
 * @param[in] ctx
 * @param[in] os
 */
int start_daemon(cis1::context_interface& ctx, cis1::os_interface& os);

/**
 * \brief Makes cronexpr if possible
 * \return valid cronexpr or std::nullopt
 * @param[in] str String contains cronexpr
 * @param[out] ec
 */
std::optional<cron::cronexpr> make_cron(
        const std::string& str,
        std::error_code& ec);

/**
 * \brief Validate regexp for job list
 * \return true if regexp valid false otherwise
 * @param[in] mask
 */
bool validate_mask(const char* mask);
