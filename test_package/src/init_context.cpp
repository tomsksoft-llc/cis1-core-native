#include <gtest/gtest.h>

#include "os_mock.h"
#include "ifstream_mock.h"
#include "context.h"
#include "error_code.h"

TEST(init_context, correct)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    EXPECT_CALL(os, get_env_var("cis_base_dir"))
        .WillOnce(Return("/test/path/cis_base_dir"));

    std::filesystem::path path{"/test/path/cis_base_dir"};

    EXPECT_CALL(os, is_directory(path, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc  << "startjob=startjob\n"
        << "setparam=setparam\n"
        << "getparam=getparam\n"
        << "setvalue=setvalue\n"
        << "getvalue=getvalue\n"
        << "cis_cron=cis_cron\n";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(path / "core" / "cis.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto ctx = cis1::init_context(ec, os);

    ASSERT_NE((bool)ec, true);
    ASSERT_EQ((bool)ctx, true);
}

TEST(init_context, invalid_env)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    EXPECT_CALL(os, get_env_var("cis_base_dir"))
        .WillOnce(Return(""));

    std::error_code ec;

    auto ctx = cis1::init_context(ec, os);

    ASSERT_EQ(ec, cis1::error_code::base_dir_not_defined);
    ASSERT_EQ((bool)ctx, false);
}

TEST(init_context, no_base_dir)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    EXPECT_CALL(os, get_env_var("cis_base_dir"))
        .WillOnce(Return("/test/path/cis_base_dir"));

    std::filesystem::path path{"/test/path/cis_base_dir"};

    EXPECT_CALL(os, is_directory(path, _))
        .WillOnce(Return(false));

    std::error_code ec;

    auto ctx = cis1::init_context(ec, os);

    ASSERT_EQ(ec, cis1::error_code::base_dir_doesnt_exist);
    ASSERT_EQ((bool)ctx, false);
}

TEST(init_context, no_base_conf_file)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    EXPECT_CALL(os, get_env_var("cis_base_dir"))
        .WillOnce(Return("/test/path/cis_base_dir"));

    std::filesystem::path path{"/test/path/cis_base_dir"};

    EXPECT_CALL(os, is_directory(path, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(false));

    EXPECT_CALL(os, open_ifstream(path / "core" / "cis.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto ctx = cis1::init_context(ec, os);

    ASSERT_EQ(ec, cis1::error_code::cant_read_base_conf_file);
    ASSERT_EQ((bool)ctx, false);
}

TEST(init_context, invalid_base_conf_file)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    EXPECT_CALL(os, get_env_var("cis_base_dir"))
        .WillOnce(Return("/test/path/cis_base_dir"));

    std::filesystem::path path{"/test/path/cis_base_dir"};

    EXPECT_CALL(os, is_directory(path, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "=a";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(path / "core" / "cis.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto ctx = cis1::init_context(ec, os);

    ASSERT_EQ(ec, cis1::error_code::cant_read_base_conf_file);
    ASSERT_EQ((bool)ctx, false);
}

