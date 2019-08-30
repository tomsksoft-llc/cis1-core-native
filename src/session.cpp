#include "session.h"

#include <chrono>
#include <sstream>
#include <iomanip>

#include <boost/process.hpp>

namespace cis1
{

session::session(
        const std::string& session_id,
            bool opened_by_me)
    : session_id_(session_id)
    , opened_by_me_(opened_by_me)
{}

bool session::opened_by_me() const
{
    return opened_by_me_;
}

const std::string& session::session_id() const
{
    return session_id_;
}

std::optional<session> invoke_session(
        context_interface& ctx,
        std::error_code& ec,
        const os_interface& os)
{
    auto session_id = os.get_env_var("session_id");

    bool session_opened_by_me = true;

    if(!session_id.empty())
    {
        session_opened_by_me = false;
    }
    else
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto id = boost::this_process::get_id();
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d-%H-%M-%S-") << id; // << ppid FIXME
        session_id = ss.str();
    }

    ctx.set_env("session_id", session_id);
    ctx.set_env(
            "session_opened_by_me",
            session_opened_by_me ? "true" : "false");

    return session{session_id, session_opened_by_me};
}

} // namespace cis1