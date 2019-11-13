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
     * @param[in] key environment variable
     * @param[in] val value
     */
    virtual void set_env(
            const std::string& key,
            const std::string& val) = 0;

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
    virtual size_t pid() const = 0;

    /**
     * \brief Getter for parent process id
     * \return parent process pid
     */
    virtual size_t ppid() const = 0;
};

} // namespace cis1
