#include <gtest/gtest.h>

#include "get_param.h"
#include "os_mock.h"
#include "context_mock.h"
#include "session_mock.h"
#include "ifstream_mock.h"

#define VALUE_NAME "value name'@!#$%^&*()-_+=\\|/~{}[]`/?\"<>:,"
#define VALUE "value '@!#$%^&*()-_+=\\|/~{}[]`/?\"<>:,"
#define ENCODED_VALUE_NAME \
    "value%20name%27%40%21%23%24%25%5E%26%2A%28%29-_%2B%3D%5C%7C%2F~%7B%7D%5B%5D%60%2F%3F%22%3C%3E%3A%2C"
#define ENCODED_VALUE \
    "value%20%27%40%21%23%24%25%5E%26%2A%28%29-_%2B%3D%5C%7C%2F~%7B%7D%5B%5D%60%2F%3F%22%3C%3E%3A%2C"
// the % should be followed by a two-digit hexadecimal number
#define INVALID_VALUE "value%2%27%40"

TEST(get_param, correct)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;
    StrictMock<os_mock> os;

    std::filesystem::path base_dir = "/";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    EXPECT_CALL(os, get_env_var("job_name"))
        .WillOnce(Return("test_job"));

    EXPECT_CALL(os, get_env_var("build_number"))
        .WillOnce(Return("000001"));

    EXPECT_CALL(
            os,
            exists( base_dir / "jobs" / "test_job" / "000001" / "job.params",
                    _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << ENCODED_VALUE_NAME "=" ENCODED_VALUE;

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(
            os,
            open_ifstream(
                    base_dir / "jobs" / "test_job" / "000001" / "job.params",
                    _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto result = cis1::get_param(ctx, session, VALUE_NAME, ec, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ((bool)result, true);
    ASSERT_EQ(result.value(), VALUE);
}

TEST(get_param, no_file)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;
    StrictMock<os_mock> os;

    std::filesystem::path base_dir = "/";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    EXPECT_CALL(os, get_env_var("job_name"))
        .WillOnce(Return("test_job"));

    EXPECT_CALL(os, get_env_var("build_number"))
        .WillOnce(Return("000001"));

    EXPECT_CALL(
            os,
            exists( base_dir / "jobs" / "test_job" / "000001" / "job.params",
                    _))
        .WillOnce(Return(false));

    std::error_code ec;

    auto result = cis1::get_param(ctx, session, "test_value", ec, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ((bool)result, true);
    ASSERT_EQ(result.value(), "");
}

TEST(get_param, invalid_file)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;
    StrictMock<os_mock> os;

    std::filesystem::path base_dir = "/";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    EXPECT_CALL(os, get_env_var("job_name"))
        .WillOnce(Return("test_job"));

    EXPECT_CALL(os, get_env_var("build_number"))
        .WillOnce(Return("000001"));

    EXPECT_CALL(
            os,
            exists( base_dir / "jobs" / "test_job" / "000001" / "job.params",
                    _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "=test";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(
            os,
            open_ifstream(
                    base_dir / "jobs" / "test_job" / "000001" / "job.params",
                    _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto result = cis1::get_param(ctx, session, "test_value", ec, os);

    ASSERT_EQ((bool)ec, true);
    ASSERT_EQ((bool)result, false);
}

TEST(get_param, invalid_value)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;
    StrictMock<os_mock> os;

    std::filesystem::path base_dir = "/";

    EXPECT_CALL(ctx, base_dir())
            .WillOnce(ReturnRef(base_dir));

    EXPECT_CALL(os, get_env_var("job_name"))
            .WillOnce(Return("test_job"));

    EXPECT_CALL(os, get_env_var("build_number"))
            .WillOnce(Return("000001"));

    EXPECT_CALL(
            os,
            exists( base_dir / "jobs" / "test_job" / "000001" / "job.params",
                    _))
            .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
            .WillOnce(Return(true));

    std::stringstream fc;

    fc << ENCODED_VALUE_NAME "=" INVALID_VALUE;

    EXPECT_CALL(*ss, istream())
            .WillOnce(ReturnRef(fc));

    EXPECT_CALL(
            os,
            open_ifstream(
                    base_dir / "jobs" / "test_job" / "000001" / "job.params",
                    _))
            .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto result = cis1::get_param(ctx, session, VALUE_NAME, ec, os);

    ASSERT_EQ((bool)ec, true);
    ASSERT_EQ((bool)result, false);
}
