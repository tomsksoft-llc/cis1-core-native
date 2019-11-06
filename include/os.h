#pragma once

#include "os_interface.h"

namespace cis1
{

/**
 * \brief Default os calls implementation
 */
class os
    : public os_interface
{
public:
     std::unique_ptr<os_interface> clone() const override;

     std::string get_env_var(
            const std::string& name) const override;

     bool is_directory(
            const std::filesystem::path& dir,
            std::error_code& ec) const override;

     bool exists(
            const std::filesystem::path& path,
            std::error_code& ec) const override;

     std::vector<
            std::unique_ptr<fs_entry_interface>> list_directory(
            const std::filesystem::path& path) const override;

     bool create_directory(
            const std::filesystem::path& dir,
            std::error_code& ec) const override;

     void copy(
            const std::filesystem::path& from,
            const std::filesystem::path& to,
            std::error_code& ec) const override;

     std::unique_ptr<ifstream_interface> open_ifstream(
            const std::filesystem::path& path,
            std::ios_base::openmode mode) const override;

     std::unique_ptr<ofstream_interface> open_ofstream(
            const std::filesystem::path& path,
            std::ios_base::openmode mode = std::ios_base::out) const override;

    void spawn_process(
            const std::string& start_dir,
            const std::string& executable,
            const std::vector<std::string>& args,
            boost::process::environment env) const override;
};

} // namespace cis1
