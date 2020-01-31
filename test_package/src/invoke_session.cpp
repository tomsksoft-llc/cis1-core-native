#include <gtest/gtest.h>

#include "session.h"
#include "os_mock.h"
#include "context_mock.h"

TEST(invoke_session, opened_by_me)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    EXPECT_CALL(os, get_env_var("session_id"))
        .WillOnce(Return(""));

    StrictMock<context_mock> ctx;

    EXPECT_CALL(ctx, set_env_var("session_id", _))
        .Times(1);

    EXPECT_CALL(ctx, set_env_var("session_opened_by_me", "true"))
        .Times(1);

    auto session = cis1::invoke_session(ctx, false, os);

    ASSERT_EQ(session.opened_by_me(), true);
}

TEST(invoke_session, opened_by_smbd)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    EXPECT_CALL(os, get_env_var("session_id"))
        .WillOnce(Return("test_session"));

    StrictMock<context_mock> ctx;

    EXPECT_CALL(ctx, set_env_var("session_id", "test_session"))
        .Times(1);

    EXPECT_CALL(ctx, set_env_var("session_opened_by_me", "false"))
        .Times(1);

    auto session = cis1::invoke_session(ctx, false, os);

    ASSERT_EQ(session.opened_by_me(), false);
}

TEST(invoke_session, force)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    StrictMock<context_mock> ctx;

    EXPECT_CALL(ctx, set_env_var("session_id", _))
        .Times(1);

    EXPECT_CALL(ctx, set_env_var("session_opened_by_me", "true"))
        .Times(1);

    auto session = cis1::invoke_session(ctx, true, os);

    ASSERT_EQ(session.opened_by_me(), true);
}
