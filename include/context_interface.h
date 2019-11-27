/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <string>
#include <filesystem>

#include <boost/process.hpp>

namespace cis1
{

/**
 * \brief Represents CIS itself and process info
 */
struct context_interface
{
    virtual ~context_interface() = default;

    /**
     * \brief Set environment variable locally
     * @param[in] key environment variable name
     * @param[in] val value
     */
    virtual void set_env_var(
            const std::string& key,
            const std::string& val) = 0;

    /**
     * \brief Getter for environment variable
     * @param[in] key environment variable name
     * \return environment variable if found empty string otherwise
     */
    virtual std::string get_env_var(
            const std::string& key) = 0;

    /**
     * \brief Getter for environment
     * \return environment merged from process and locally set variables
     */
    virtual const boost::process::environment& env() const = 0;

    /**
     * \brief Getter for CIS base directory
     * \return CIS base directory
     */
    virtual const std::filesystem::path& base_dir() const = 0;

    /**
     * \brief Getter for process id
     * \return current process id
     */
    virtual size_t process_id() const = 0;

    /**
     * \brief Getter for parent startjob id
     * \return parent startjob id
     */
    virtual size_t parent_startjob_id() const = 0;
};

} // namespace cis1
