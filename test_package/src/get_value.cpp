#include <gtest/gtest.h>

#include "get_value.h"
#include "os_mock.h"
#include "context_mock.h"
#include "session_mock.h"
#include "ifstream_mock.h"

#define VALUE_NAME "origin\nvalue"
#define VALUE R"(2 = 4 \ 2)"
#define ENCODED_VALUE_NAME R"(origin\nvalue)"
#define ENCODED_VALUE R"(2 \= 4 \\ 2)"
// Unknown "\?" escaping
#define INVALID_VALUE R"(2 \= \? 2)"

TEST(get_value, correct)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;
    StrictMock<os_mock> os;

    std::filesystem::path base_dir = "/";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    std::string session_id = "test_id";

    EXPECT_CALL(session, session_id())
        .WillOnce(ReturnRef(session_id));

    EXPECT_CALL(os, exists(base_dir / "sessions" / "test_id.dat", _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << ENCODED_VALUE_NAME "=" ENCODED_VALUE;

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(base_dir / "sessions" / "test_id.dat", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto result = cis1::get_value(ctx, session, VALUE_NAME, ec, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ((bool)result, true);
    ASSERT_EQ(result.value(), VALUE);
}

TEST(get_value, no_file)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;
    StrictMock<os_mock> os;

    std::filesystem::path base_dir = "/";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    std::string session_id = "test_id";

    EXPECT_CALL(session, session_id())
        .WillOnce(ReturnRef(session_id));

    EXPECT_CALL(os, exists(base_dir / "sessions" / "test_id.dat", _))
        .WillOnce(Return(false));

    std::error_code ec;

    auto result = cis1::get_value(ctx, session, "test_value", ec, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ((bool)result, true);
    ASSERT_EQ(result.value(), "");
}

TEST(get_value, invalid_file)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;
    StrictMock<os_mock> os;

    std::filesystem::path base_dir = "/";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    std::string session_id = "test_id";

    EXPECT_CALL(session, session_id())
        .WillOnce(ReturnRef(session_id));

    EXPECT_CALL(os, exists(base_dir / "sessions" / "test_id.dat", _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "=test";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(base_dir / "sessions" / "test_id.dat", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto result = cis1::get_value(ctx, session, "test_value", ec, os);

    ASSERT_EQ((bool)ec, true);
    ASSERT_EQ((bool)result, false);
}

TEST(get_value, invalid_value)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;
    StrictMock<os_mock> os;

    std::filesystem::path base_dir = "/";

    EXPECT_CALL(ctx, base_dir())
            .WillOnce(ReturnRef(base_dir));

    std::string session_id = "test_id";

    EXPECT_CALL(session, session_id())
            .WillOnce(ReturnRef(session_id));

    EXPECT_CALL(os, exists(base_dir / "sessions" / "test_id.dat", _))
            .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
            .WillOnce(Return(true));

    std::stringstream fc;

    fc << ENCODED_VALUE_NAME "=" INVALID_VALUE;

    EXPECT_CALL(*ss, istream())
            .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(base_dir / "sessions" / "test_id.dat", _))
            .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto result = cis1::get_value(ctx, session, VALUE_NAME, ec, os);

    ASSERT_EQ((bool)ec, true);
    ASSERT_EQ((bool)result, false);
}
