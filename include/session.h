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
#include <string>

#include "context_interface.h"
#include "os_interface.h"
#include "session_interface.h"

namespace cis1
{

/**
 * \brief Represents CIS session
 */
class session
    : public session_interface
{
public:
    /**
     * \brief Constructs session instance
     * @param[in] session_id Session identificator
     * @param[in] opened_by_me Flag that identifies
     *                         whether session created by this process
     */
    session(const std::string& session_id,
            bool opened_by_me);

    ~session();

    /**
     * \brief Identifies is current process session owner
     * \return \b true if session opened by current executable \b false otherwise
     */
    virtual bool opened_by_me() const override;

    /**
     * \brief Getter for session identifier
     * \return current session identifier
     */
    virtual const std::string& session_id() const override;

    /**
     * \brief Sets close handler
     * @param[in] handler
     */
    virtual void on_close(std::function<void(session_interface&)> handler) override;

private:
    const std::string session_id_;
    const bool opened_by_me_;
    std::function<void(session_interface&)> on_close_;
};

/**
 * \brief Create session if possible
 * \return valid session or std::nullopt
 * @param[in, out] ctx session env vars will be set
 * @param[out] ec
 * @param[in] os
 */
session invoke_session(
        context_interface& ctx,
        const os_interface& os);

} // namespace cis1
