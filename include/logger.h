/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <ostream>

#include "webui_session.h"
#include "context_interface.h"
#include "session_interface.h"

/**
 * \brief Initialize webui logging
 * @param[in] session
 */
void init_webui_log(std::shared_ptr<webui_session> session);

/**
 * \brief Initialize global logging
 * @param[in] ctx
 */
void init_cis_log(
        const cis1::context_interface& ctx);

/**
 * \brief Initialize session logging
 * @param[in] ctx
 * @param[in] session
 */
void init_session_log(
        const cis1::context_interface& ctx,
        const cis1::session_interface& session);

/**
 * \brief Prepare cis log to write and get write handle
 * \return ostream to cis log
 *
 * User should end message with std::endl\n
 * If cis log is not initialized doesn nothing.\n
 * If webui log initialized also writes to webui log
 */
std::ostream& cis_log();

/**
 * \brief Prepare session log to write and get write handle
 * \return ostream to session log
 *
 * User should end message with std::endl\n
 * If session log is not initialized doesn nothing.\n
 * If webui log initialized also writes to webui log
 */
std::ostream& session_log();

/**
 * \brief Prepare webui log to write and get write handle
 * \return ostream to webui log
 *
 * User should end message with std::endl\n
 * If webui log is not initialized doesn nothing.
 */
std::ostream& webui_log();

/**
 * \brief Prepare all logs to write and get write handle
 * \return ostream to all logs
 *
 * User should end message with std::endl
 */
std::ostream& tee_log();
