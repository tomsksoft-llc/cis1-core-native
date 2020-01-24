/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <optional>
#include <system_error>
#include <map>
#include <string>
#include <filesystem>

#include <boost/process.hpp>

#include "context_interface.h"
#include "os_interface.h"

namespace cis1
{

/**
 * \brief Main implementation of CIS itself
 */
class context
    : public context_interface
{
public:
    /**
     * \brief Constructs context instance
     * @param[in] base_dir CIS directory
     * @param[in] executables CIS names-to-executables map
     */
    context(const std::filesystem::path& base_dir,
            const std::map<std::string, std::string>& executables);

    /**
     * \brief Set environment variable locally
     * @param[in] key environment variable name
     * @param[in] val value
     */
    virtual void set_env_var(
            const std::string& key,
            const std::string& val) override;

    /**
     * \brief Getter for environment variable
     * @param[in] key environment variable name
     * \return environment variable if found empty string otherwise
     */
    std::string get_env_var(
            const std::string& key) override;

    /**
     * \brief Getter for environment
     * \return environment merged from process and locally set variables
     */
    virtual const boost::process::environment& env() const override;

    /**
     * \brief Getter for CIS base directory
     * \return CIS base directory
     */
    virtual const std::filesystem::path& base_dir() const override;

    /**
     * \brief Getter for process id
     * \return current process id
     */
    virtual size_t process_id() const override;

    /**
     * \brief Getter for parent startjob id
     * \return parent startjob id
     */
    virtual size_t parent_startjob_id() const override;

private:
    const std::filesystem::path base_dir_;
    const std::map<std::string, std::string> executables_;
    boost::process::environment env_;
    size_t process_id_;
    size_t parent_startjob_id_;
};

/**
 * \brief Creates context if possible
 * \return valid context or std::nullopt
 * @param[out] ec
 * @param[in] os
 */
std::optional<context> init_context(
        std::error_code& ec,
        const os_interface& os);

} // namespace cis1
