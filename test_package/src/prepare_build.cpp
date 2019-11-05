#include <gtest/gtest.h>

#include "build.h"
#include "os_mock.h"
#include "context_mock.h"
#include "error_code.h"
#include "ifstream_mock.h"
#include "utils.h"

TEST(prepare_build, job_doesnt_exist)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, set_env("job_name", "test_job"))
        .Times(1);

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    EXPECT_CALL(os, is_directory(base_dir / "jobs" / "test_job", _))
        .WillOnce(Return(false));

    std::error_code ec;

    auto build = cis1::prepare_build(ctx, "test_job", ec, os);

    ASSERT_EQ(ec, cis1::error_code::job_dir_doesnt_exist);
    ASSERT_EQ((bool)build, false);
}

TEST(prepare_build, no_job_conf)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, set_env("job_name", "test_job"))
        .Times(1);

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, is_directory(job_dir, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(false));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto build = cis1::prepare_build(ctx, "test_job", ec, os);

    ASSERT_EQ(ec, cis1::error_code::cant_read_job_conf_file);
    ASSERT_EQ((bool)build, false);
}

TEST(prepare_build, invalid_job_conf)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, set_env("job_name", "test_job"))
        .Times(1);

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, is_directory(job_dir, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::error_code ec;

    auto build = cis1::prepare_build(ctx, "test_job", ec, os);

    ASSERT_EQ(ec, cis1::error_code::cant_read_job_conf_file);
    ASSERT_EQ((bool)build, false);
}

TEST(prepare_build, script_doesnt_exist)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, set_env("job_name", "test_job"))
        .Times(1);

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, is_directory(job_dir, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "script=test_script";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(job_dir / "job.conf", _))
        .WillOnce(Return(ByMove(std::move(ss))));

    EXPECT_CALL(os, exists(job_dir / "test_script", _))
        .WillOnce(Return(false));

    std::error_code ec;

    auto build = cis1::prepare_build(ctx, "test_job", ec, os);

    ASSERT_EQ(ec, cis1::error_code::script_doesnt_exist);
    ASSERT_EQ((bool)build, false);
}

TEST(prepare_build, job_without_params)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, set_env("job_name", "test_job"))
        .Times(1);

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, is_directory(job_dir, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "script=test_script";

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

    std::error_code ec;

    auto build = cis1::prepare_build(ctx, "test_job", ec, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ((bool)build, true);
}

TEST(prepare_build, invalid_job_params)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, set_env("job_name", "test_job"))
        .Times(1);

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, is_directory(job_dir, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "script=test_script";

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

    auto build = cis1::prepare_build(ctx, "test_job", ec, os);

    ASSERT_EQ(ec, cis1::error_code::cant_read_job_params_file);
    ASSERT_EQ((bool)build, false);
}

TEST(prepare_build, correct)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, set_env("job_name", "test_job"))
        .Times(1);

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    auto job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, is_directory(job_dir, _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    fc << "script=test_script";

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

    std::error_code ec;

    auto build = cis1::prepare_build(ctx, "test_job", ec, os);

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ((bool)build, true);
    ASSERT_THAT(build->params(), ElementsAreArray(params));
}
