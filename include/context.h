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
     * @param[in] key environment variable
     * @param[in] val value
     */
    virtual void set_env(
            const std::string& key,
            const std::string& val) override;

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
    virtual size_t pid() const override;

    /**
     * \brief Getter for parent process id
     * \return parent process pid
     */
    virtual size_t ppid() const override;

private:
    const std::filesystem::path base_dir_;
    const std::map<std::string, std::string> executables_;
    boost::process::environment env_;
    size_t pid_;
    size_t ppid_;
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
