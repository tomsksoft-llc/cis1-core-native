#pragma once

#include <filesystem>
#include <string>

#include <boost/process.hpp>

#include <gmock/gmock.h>

class sys_stream_mock
{
public:
    MOCK_CONST_METHOD0(
            close,
            sys_stream_mock&());

    MOCK_CONST_METHOD1(
            less_op,
            sys_stream_mock&(boost::process::async_pipe&));

    sys_stream_mock& operator>(boost::process::async_pipe& pipe) const
    {
        return less_op(pipe);
    }
};

class process_mock
{
public:
    MOCK_CONST_METHOD8(async_system,
            void(
                    boost::asio::io_context& io_ctx,
                    std::function<void(
                            boost::system::error_code err,
                            int exit_code)>&& cb,
                    std::string cmd,
                    boost::process::environment env,
                    std::filesystem::path path,
                    sys_stream_mock& std_in,
                    sys_stream_mock& std_out,
                    sys_stream_mock& std_err));

    MOCK_CONST_METHOD0(std_in, sys_stream_mock&());

    MOCK_CONST_METHOD0(std_out, sys_stream_mock&());

    MOCK_CONST_METHOD0(std_err, sys_stream_mock&());
};

