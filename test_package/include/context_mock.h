#pragma once

#include <gmock/gmock.h>

#include "context_interface.h"

class context_mock
    : public cis1::context_interface
{
public:
    MOCK_METHOD2(
            set_env_var,
            void(const std::string& var, const std::string& value));

    MOCK_METHOD1(
            get_env_var,
            std::string(const std::string& var));

    MOCK_CONST_METHOD0(
            env,
            const boost::process::environment&());

    MOCK_CONST_METHOD0(
            base_dir,
            const std::filesystem::path&());

    MOCK_CONST_METHOD0(
            process_id,
            size_t());

    MOCK_CONST_METHOD0(
            parent_startjob_id,
            size_t());
};
