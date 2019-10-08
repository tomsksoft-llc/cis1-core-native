#pragma once

#include <ostream>

#include "webui_session.h"
#include "context_interface.h"
#include "session_interface.h"

void init_webui_log(std::shared_ptr<webui_session> session);

void init_cis_log(
        const cis1::context_interface& ctx);

void init_session_log(
        const cis1::context_interface& ctx,
        const cis1::session_interface& session);

std::ostream& cis_log();

std::ostream& session_log();

std::ostream& webui_log();

std::ostream& tee_log();
