#pragma once

#include <string>

#include "context_interface.h"
#include "os_interface.h"

int clean_job(
        const std::string& job_name,
        cis1::context_interface& ctx,
        cis1::os_interface& os);
