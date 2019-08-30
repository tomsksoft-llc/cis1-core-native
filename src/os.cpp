#include "os.h"

#include <boost/process.hpp>

#include "ifstream_adapter.h"
#include "ofstream_adapter.h"
#include "fs_entry_adapter.h"

namespace cis1
{

std::unique_ptr<os_interface> os::clone() const
{
    return std::make_unique<os>();
}

std::string os::get_env_var(const std::string& name) const
{
    auto env = boost::this_process::environment();

    auto it = env.find(name);

    if(it != env.end())
    {
        return it->to_vector()[0];
    }

    return {};
}

bool os::is_directory(
        const std::filesystem::path& dir,
        std::error_code& ec) const
{
    return std::filesystem::is_directory(dir, ec);
}

bool os::exists(
        const std::filesystem::path& path,
        std::error_code& ec) const
{
    return std::filesystem::exists(path, ec);
}

std::vector<
        std::unique_ptr<fs_entry_interface>> os::list_directory(
        const std::filesystem::path& path) const
{
    std::vector<std::unique_ptr<fs_entry_interface>> result;

    for(auto& entry : std::filesystem::directory_iterator(path))
    {
        result.push_back(std::make_unique<fs_entry_adapter>(entry));
    }

    return result;
}

bool os::create_directory(
        const std::filesystem::path& dir,
        std::error_code& ec) const
{
    return std::filesystem::create_directory(dir, ec);
}

void os::copy(
        const std::filesystem::path& from,
        const std::filesystem::path& to,
        std::error_code& ec) const
{
    return std::filesystem::copy(from, to, ec);
}

std::unique_ptr<ifstream_interface> os::open_ifstream(
        const std::filesystem::path& path,
        std::ios_base::openmode mode) const
{
    return std::make_unique<ifstream_adapter>(path, mode);
}

std::unique_ptr<ofstream_interface> os::open_ofstream(
        const std::filesystem::path& path,
        std::ios_base::openmode mode) const
{
    return std::make_unique<ofstream_adapter>(path, mode);
}

} // namespace cis1
