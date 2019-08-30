#include <gtest/gtest.h>

#include "set_param.h"
#include "os_mock.h"
#include "context_mock.h"
#include "session_mock.h"
#include "ifstream_mock.h"
#include "ofstream_mock.h"

TEST(set_param, correct)
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
            exists( base_dir / "sessions" / (session_id + ".prm"),
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
                    base_dir / "sessions" / (session_id + ".prm"),
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
                    base_dir / "sessions" / (session_id + ".prm"),
                    _))
        .WillOnce(Return(ByMove(std::move(oss))));

    std::error_code ec;

    cis1::set_param(ctx, session, "test_value", "value", ec, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_STREQ(fc2.str().c_str(), "test_value=value\n");
}

TEST(set_param, no_file)
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
            exists( base_dir / "sessions" / (session_id + ".prm"),
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
                    base_dir / "sessions" / (session_id + ".prm"),
                    _))
        .WillOnce(Return(ByMove(std::move(oss))));

    std::error_code ec;

    cis1::set_param(ctx, session, "test_value", "value", ec, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_STREQ(fc2.str().c_str(), "test_value=value\n");
}

TEST(set_param, invalid_file)
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
            exists( base_dir / "sessions" / (session_id + ".prm"),
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
                    base_dir / "sessions" / (session_id + ".prm"),
                    _))
        .WillOnce(Return(ByMove(std::move(iss))));

    std::error_code ec;

    cis1::set_param(ctx, session, "test_value", "value", ec, os);

    ASSERT_EQ((bool)ec, true);
}
