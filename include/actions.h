/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

// the longest action name is close_session
const std::size_t max_action_name_length = 13;

enum class actions
{
    error,
    getvalue,
    setvalue,
    getparam,
    setparam,
    open_session,
    close_session,
    start_job,
    finish_job,
    startjob_stdout,
    startjob_stderr,
};

inline std::string ToString(const actions action)
{
    switch(action)
    {
        case actions::error:
            return "error";
        case actions::getvalue:
            return "getvalue";
        case actions::setvalue:
            return "setvalue";
        case actions::getparam:
            return "getparam";
        case actions::setparam:
            return "setparam";
        case actions::open_session:
            return "open_session";
        case actions::close_session:
            return "close_session";
        case actions::start_job:
            return "start_job";
        case actions::finish_job:
            return "finish_job";
        case actions::startjob_stdout:
            return "startjob_stdout";
        case actions::startjob_stderr:
            return "startjob_stderr";
        default:
            return "unknown";
    }
}
