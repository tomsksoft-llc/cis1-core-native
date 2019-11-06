#pragma once

#include <gmock/gmock.h>

#include "os_interface.h"

class os_mock
    : public cis1::os_interface
{
public:
    MOCK_CONST_METHOD0(
            clone,
            std::unique_ptr<cis1::os_interface>());

    MOCK_CONST_METHOD1(
            get_env_var,
            std::string(const std::string& name));

    MOCK_CONST_METHOD2(
            is_directory,
            bool(const std::filesystem::path& dir, std::error_code& ec));

    MOCK_CONST_METHOD2(
            exists,
            bool(const std::filesystem::path& path, std::error_code& ec));

    MOCK_CONST_METHOD1(
            list_directory,
            std::vector<
            std::unique_ptr<cis1::fs_entry_interface>>(
            const std::filesystem::path& path));

    MOCK_CONST_METHOD2(
            create_directory,
            bool(const std::filesystem::path& dir, std::error_code& ec));

    MOCK_CONST_METHOD3(
            copy,
            void(   const std::filesystem::path& from,
                    const std::filesystem::path& to,
                    std::error_code& ec));

    MOCK_CONST_METHOD2(
            open_ifstream,
            std::unique_ptr<cis1::ifstream_interface>(
                    const std::filesystem::path& path,
                    std::ios_base::openmode mode));

    MOCK_CONST_METHOD2(
            open_ofstream,
            std::unique_ptr<cis1::ofstream_interface>(
                    const std::filesystem::path& path,
                    std::ios_base::openmode mode));

    MOCK_CONST_METHOD4(
            spawn_process,
            void(   const std::string& start_dir,
                    const std::string& executable,
                    const std::vector<std::string>& args,
                    boost::process::environment env));
};
