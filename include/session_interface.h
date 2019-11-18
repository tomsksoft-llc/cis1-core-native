/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <string>

namespace cis1
{


/**
 * \brief Interface of CIS session
 */
struct session_interface
{
    virtual ~session_interface() = default;

    /**
     * \brief Identifies is current process session owner
     * \return \b true if session opened by current executable \b false otherwise
     */
    virtual bool opened_by_me() const = 0;

    /**
     * \brief Getter for session identifier
     * \return current session identifier
     */
    virtual const std::string& session_id() const = 0;
};

} // namespace cis1
