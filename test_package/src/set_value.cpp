#include <gtest/gtest.h>

#include "set_value.h"
#include "os_mock.h"
#include "context_mock.h"
#include "session_mock.h"
#include "ifstream_mock.h"
#include "ofstream_mock.h"

#define VALUE_NAME "value name'@!#$%^&*()-_+=\\|/~{}[]`/?\"<>:,"
#define VALUE "value '@!#$%^&*()-_+=\\|/~{}[]`/?\"<>:,"
#define ENCODED_VALUE_NAME \
    "value%20name%27%40%21%23%24%25%5E%26%2A%28%29-_%2B%3D%5C%7C%2F~%7B%7D%5B%5D%60%2F%3F%22%3C%3E%3A%2C"
#define ENCODED_VALUE \
    "value%20%27%40%21%23%24%25%5E%26%2A%28%29-_%2B%3D%5C%7C%2F~%7B%7D%5B%5D%60%2F%3F%22%3C%3E%3A%2C"

TEST(set_value, correct)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;
    StrictMock<os_mock> os;

    std::filesystem::path base_dir = "/";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    std::string session_id = "test_session";

    EXPECT_CALL(session, session_id())
        .WillOnce(ReturnRef(session_id));

    EXPECT_CALL(
            os,
            exists( base_dir / "sessions" / (session_id + ".dat"),
                    _))
        .WillOnce(Return(true));

    auto iss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*iss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    EXPECT_CALL(*iss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(
            os,
            open_ifstream(
                    base_dir / "sessions" / (session_id + ".dat"),
                    _))
        .WillOnce(Return(ByMove(std::move(iss))));

    auto oss = std::make_unique<StrictMock<ofstream_mock>>();

    EXPECT_CALL(*oss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc2;

    EXPECT_CALL(*oss, ostream())
        .WillOnce(ReturnRef(fc2));

    EXPECT_CALL(
            os,
            open_ofstream(
                    base_dir / "sessions" / (session_id + ".dat"),
                    _))
        .WillOnce(Return(ByMove(std::move(oss))));

    std::error_code ec;

    cis1::set_value(ctx, session, VALUE_NAME, VALUE, ec, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_STREQ(fc2.str().c_str(), ENCODED_VALUE_NAME "=" ENCODED_VALUE "\n");
}

TEST(set_value, no_file)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;
    StrictMock<os_mock> os;

    std::filesystem::path base_dir = "/";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    std::string session_id = "test_session";

    EXPECT_CALL(session, session_id())
        .WillOnce(ReturnRef(session_id));

    EXPECT_CALL(
            os,
            exists( base_dir / "sessions" / (session_id + ".dat"),
                    _))
        .WillOnce(Return(false));

    auto oss = std::make_unique<StrictMock<ofstream_mock>>();

    EXPECT_CALL(*oss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc2;

    EXPECT_CALL(*oss, ostream())
        .WillOnce(ReturnRef(fc2));

    EXPECT_CALL(
            os,
            open_ofstream(
                    base_dir / "sessions" / (session_id + ".dat"),
                    _))
        .WillOnce(Return(ByMove(std::move(oss))));

    std::error_code ec;

    cis1::set_value(ctx, session, "test_value", "value", ec, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_STREQ(fc2.str().c_str(), "test_value=value\n");
}

TEST(set_value, invalid_file)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;
    StrictMock<os_mock> os;

    std::filesystem::path base_dir = "/";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    std::string session_id = "test_session";

    EXPECT_CALL(session, session_id())
        .WillOnce(ReturnRef(session_id));

    EXPECT_CALL(
            os,
            exists( base_dir / "sessions" / (session_id + ".dat"),
                    _))
        .WillOnce(Return(true));

    auto iss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*iss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "=test";

    EXPECT_CALL(*iss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(
            os,
            open_ifstream(
                    base_dir / "sessions" / (session_id + ".dat"),
                    _))
        .WillOnce(Return(ByMove(std::move(iss))));

    std::error_code ec;

    cis1::set_value(ctx, session, "test_value", "value", ec, os);

    ASSERT_EQ((bool)ec, true);
}
