#include <gtest/gtest.h>

#include "job.h"
#include "os_mock.h"
#include "context_mock.h"
#include "error_code.h"
#include "ifstream_mock.h"
#include "utils.h"

TEST(load_job, job_doesnt_exist)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    EXPECT_CALL(os, exists(base_dir / "jobs" / "test_job", _))
        .WillOnce(Return(false));

    std::error_code ec;

    auto job_opt = cis1::load_job("test_job", ec, ctx, os);

    ASSERT_EQ(ec, cis1::error_code::job_dir_doesnt_exist);
    ASSERT_EQ((bool)job_opt, false);
}

TEST(load_job, no_job_conf)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, exists(base_dir / "jobs" / "test_job", _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto job_opt = cis1::load_job("test_job", ec, ctx, os);

    ASSERT_EQ(ec, cis1::error_code::cant_read_job_conf_file);
    ASSERT_EQ((bool)job_opt, false);
}

TEST(load_job, invalid_job_conf)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, exists(job_dir, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    EXPECT_CALL(*ss, istream())
        .WillRepeatedly(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto job_opt = cis1::load_job("test_job", ec, ctx, os);

    ASSERT_EQ(ec, cis1::error_code::cant_read_job_conf_file);
    ASSERT_EQ((bool)job_opt, false);
}

TEST(load_job, script_doesnt_exist)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, exists(job_dir, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "script=test_script\n";
    fc << "keep_last_success_builds=5\n";
    fc << "keep_last_break_builds=5";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    EXPECT_CALL(os, exists(job_dir / "test_script", _))
        .WillOnce(Return(false));

    std::error_code ec;

    auto job_opt = cis1::load_job("test_job", ec, ctx, os);

    ASSERT_EQ(ec, cis1::error_code::script_doesnt_exist);
    ASSERT_EQ((bool)job_opt, false);
}

TEST(load_job, job_without_params)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, exists(job_dir, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "script=test_script\n";
    fc << "keep_last_success_builds=5\n";
    fc << "keep_last_break_builds=5";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    EXPECT_CALL(os, exists(job_dir / "test_script", _))
        .WillOnce(Return(true));

    ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(false));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.params", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    EXPECT_CALL(os, list_directory(job_dir))
        .WillOnce(Return(ByMove(
                    std::vector<std::unique_ptr<cis1::fs_entry_interface>>{})));

    std::error_code ec;

    auto job_opt = cis1::load_job("test_job", ec, ctx, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ((bool)job_opt, true);
}

TEST(load_job, invalid_job_params)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, exists(job_dir, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "script=test_script\n";
    fc << "keep_last_success_builds=5\n";
    fc << "keep_last_break_builds=5";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    EXPECT_CALL(os, exists(job_dir / "test_script", _))
        .WillOnce(Return(true));

    ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc2;

    fc2 << "=a";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc2));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.params", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto job_opt = cis1::load_job("test_job", ec, ctx, os);

    ASSERT_EQ(ec, cis1::error_code::cant_read_job_params_file);
    ASSERT_EQ((bool)job_opt, false);
}

TEST(load_job, correct)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, exists(job_dir, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "script=test_script\n";
    fc << "keep_last_success_builds=5\n";
    fc << "keep_last_break_builds=5";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    EXPECT_CALL(os, exists(job_dir / "test_script", _))
        .WillOnce(Return(true));

    ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::vector<std::pair<std::string, std::string>> params =
    {
        {"arg2", "test"},
        {"arg1", ""}
    };

    std::stringstream fc2;

    fc2 << "arg2=test\n"
              << "arg1=";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc2));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.params", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    EXPECT_CALL(os, list_directory(job_dir))
        .WillOnce(Return(ByMove(
                    std::vector<std::unique_ptr<cis1::fs_entry_interface>>{})));
    
    std::error_code ec;

    auto job_opt = cis1::load_job("test_job", ec, ctx, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ((bool)job_opt, true);
    ASSERT_THAT(job_opt->params(), ElementsAreArray(params));
}
