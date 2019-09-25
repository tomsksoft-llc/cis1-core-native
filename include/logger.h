#pragma once

#include <ostream>

#include "context_interface.h"
#include "session_interface.h"

void init_cis_log(
        const cis1::context_interface& ctx);

void init_session_log(
        const cis1::context_interface& ctx,
        const cis1::session_interface& session);

std::ostream& cis_log();

std::ostream& session_log();

std::ostream& tie_log();
