/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#pragma once

#include <scl/logger.h>
#include <scf/scf.h>

#include "webui_session.h"
#include "context_interface.h"
#include "session_interface.h"
#include "os.h"

#include "actions.h"

#define CIS_LOG(action, format, ...) cis_log(action, SCFormat(format, ##__VA_ARGS__))
#define SES_LOG(action, format, ...) session_log(action, SCFormat(format, ##__VA_ARGS__))
#define WEBUI_LOG(action, format, ...) webui_log(action, SCFormat(format, ##__VA_ARGS__))
#define TEE_LOG(action, format, ...) tee_log(action, SCFormat(format, ##__VA_ARGS__))

/**
 * \brief Make Logger options by cis context and os
 * @param[in] options
 * @param[in] session
 */
scl::Logger::Options make_logger_options(
        const std::optional<std::string>& session_id,
        const cis1::context_interface& ctx,
        const cis1::os& std_os);

/**
 * \brief Initialize webui logging
 * @param[in] options
 * @param[in] session
 */
void init_webui_log(
        const scl::Logger::Options& options,
        const std::shared_ptr<webui_session>& session);

/**
 * \brief Initialize global logging
 * @param[in] options
 * @param[in] ctx
 */
void init_cis_log(
        const scl::Logger::Options& options,
        const cis1::context_interface& ctx);

/**
 * \brief Initialize session logging
 * @param[in] options
 * @param[in] ctx
 * @param[in] session
 */
void init_session_log(
        const scl::Logger::Options& options,
        const cis1::context_interface& ctx,
        const cis1::session_interface& session);


/**
 * \brief Record the message to an cis log
 *
 * If cis log is not initialized doesn nothing.\n
 * If an webui log initialized also writes to webui log
 */
void cis_log(actions act, const std::string& message);

/**
 * \brief Record the message to an session log
 *
 * If session log is not initialized doesn nothing.\n
 * If an webui log initialized also writes to webui log
 */
void session_log(actions act, const std::string& message);

/**
 * \brief Record the message to an webui log
 *
 * If an webui log is not initialized do not nothing.
 */
void webui_log(actions act, const std::string& message);

/**
 * \brief Record the message to session and cis logs
 *
 * If an webui log initialized also writes to webui log
 */
void tee_log(actions act, const std::string& message);

extern scl::LoggerPtr cis_logger;
extern scl::LoggerPtr session_logger;
extern scl::LoggerPtr webui_logger;
