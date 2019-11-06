#include <gtest/gtest.h>

#include <sstream>

#include "cron.h"

#include "os_mock.h"
#include "ifstream_mock.h"
#include "context_mock.h"

TEST(cron_list, load_incorrect)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    std::filesystem::path crons_path = "/test/crons";

    std::error_code ec;

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(os, open_ifstream(crons_path, _))
        .WillOnce(Return(ByMove(std::move(ss))));

    auto cron_list = load_cron_list(crons_path, ec, os);

    ASSERT_TRUE((bool)ec);
    ASSERT_FALSE((bool)cron_list);
}

TEST(cron_list, load_correct)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    std::filesystem::path crons_path = "/test/crons";

    std::error_code ec;

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillRepeatedly(Return(true));

    std::stringstream fc;

    std::string job1 = "internal/core_test";
    std::string cron1 = "* * * * * *";

    std::string job2 = "internal/test";
    std::string cron2 = "*/12 * * * * *";

    fc << job1 << " " << cron1 << std::endl;
    fc << job2 << " " << cron2;

    EXPECT_CALL(*ss, istream())
        .WillRepeatedly(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(crons_path, _))
        .WillOnce(Return(ByMove(std::move(ss))));

    auto cron_list = load_cron_list(crons_path, ec, os);

    std::set<cron_entry> expected_set
    {
        {job1, cron1},
        {job2, cron2}
    };

    ASSERT_FALSE((bool)ec);
    ASSERT_TRUE((bool)cron_list);
    ASSERT_TRUE(
            std::equal(
                    expected_set.begin(),
                    expected_set.end(),
                    cron_list->list().begin()));
}

TEST(cron_list, add)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    std::filesystem::path crons_path = "/test/crons";

    std::error_code ec;

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillRepeatedly(Return(true));

    std::stringstream fc;

    std::string job1 = "internal/core_test";
    std::string cron1 = "* * * * * *";

    std::string job2 = "internal/test";
    std::string cron2 = "*/12 * * * * *";

    fc << job1 << " " << cron1 << std::endl;
    fc << job2 << " " << cron2;

    EXPECT_CALL(*ss, istream())
        .WillRepeatedly(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(crons_path, _))
        .WillOnce(Return(ByMove(std::move(ss))));

    auto cron_list = load_cron_list(crons_path, ec, os);

    ASSERT_FALSE((bool)ec);
    ASSERT_TRUE((bool)cron_list);


    std::string job3 = "test/test";
    std::string cron3 = "*/12 */3 * * * *";

    cron_list->add({job3, cron3});

    std::set<cron_entry> expected_set
    {
        {job1, cron1},
        {job2, cron2},
        {job3, cron3}
    };

    ASSERT_TRUE(
            std::equal(
                    expected_set.begin(),
                    expected_set.end(),
                    cron_list->list().begin()));
}

TEST(cron_list, del)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    std::filesystem::path crons_path = "/test/crons";

    std::error_code ec;

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillRepeatedly(Return(true));

    std::stringstream fc;

    std::string job1 = "internal/core_test";
    std::string cron1 = "* * * * * *";

    std::string job2 = "internal/test";
    std::string cron2 = "*/12 * * * * *";

    fc << job1 << " " << cron1 << std::endl;
    fc << job2 << " " << cron2;

    EXPECT_CALL(*ss, istream())
        .WillRepeatedly(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(crons_path, _))
        .WillOnce(Return(ByMove(std::move(ss))));

    auto cron_list = load_cron_list(crons_path, ec, os);

    ASSERT_FALSE((bool)ec);
    ASSERT_TRUE((bool)cron_list);


    cron_list->del({job2, cron2});

    std::set<cron_entry> expected_set
    {
        {job1, cron1}
    };

    ASSERT_TRUE(
            std::equal(
                    expected_set.begin(),
                    expected_set.end(),
                    cron_list->list().begin()));
}

TEST(cron_manager, run_job)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<os_mock> os;

    boost::asio::io_context io_ctx;

    std::filesystem::path crons_path = "/test/crons";

    cron_manager cm(io_ctx, ctx, crons_path, os);

    std::string job_name = "test/test";

    boost::process::environment env;

    std::string startjob = "test_exec";

    env["startjob"] = startjob;

    EXPECT_CALL(ctx, env())
        .WillRepeatedly(ReturnRef(env));

    std::filesystem::path base_dir = "/base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillRepeatedly(ReturnRef(base_dir));

    auto executable =
            std::filesystem::path{"core"}
            / startjob;

    EXPECT_CALL(
            os,
            spawn_process(
                    base_dir.generic_string(),
                    executable.generic_string(),
                    std::vector<std::string>{job_name},
                    _))
            .Times(1);

    cm.run_job(job_name);
}

TEST(cron_manager, update)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<os_mock> os;

    boost::asio::io_context io_ctx;

    std::filesystem::path crons_path = "/test/crons";

    cron_manager cm(io_ctx, ctx, crons_path, os);

    std::string job_name = "test/test";

    boost::process::environment env;

    std::string startjob = "test_exec";

    env["startjob"] = startjob;

    EXPECT_CALL(ctx, env())
        .WillRepeatedly(ReturnRef(env));

    std::filesystem::path base_dir = "/base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillRepeatedly(ReturnRef(base_dir));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillRepeatedly(Return(true));

    std::stringstream fc;

    std::string job1 = "test/test";
    std::string cron1 = "* * * * * *";

    fc << job1 << " " << cron1;

    EXPECT_CALL(*ss, istream())
        .WillRepeatedly(ReturnRef(fc));

    EXPECT_CALL(os, open_ifstream(crons_path, _))
        .WillOnce(Return(ByMove(std::move(ss))));

    cm.update();

    ASSERT_TRUE(cm.timers_.count({job1, cron1}) == 1);
}

TEST(cron, start_daemon)
{
    using namespace ::testing;

    StrictMock<context_mock> ctx;
    StrictMock<os_mock> os;

    boost::process::environment env;

    std::string cis_cron_daemon = "test_exec";

    env["cis_cron_daemon"] = cis_cron_daemon;

    EXPECT_CALL(ctx, env())
        .WillRepeatedly(ReturnRef(env));

    std::filesystem::path base_dir = "/base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillRepeatedly(ReturnRef(base_dir));

    auto executable =
            std::filesystem::path{"core"}
            / cis_cron_daemon;

    EXPECT_CALL(
            os,
            spawn_process(
                    base_dir.generic_string(),
                    executable.generic_string(),
                    std::vector<std::string>{},
                    _))
            .Times(1);

    start_daemon(ctx, os);
}
