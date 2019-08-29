#include <gtest/gtest.h>

#include "utils.h"
#include "build.h"
#include "fs_entry_mock.h"
#include "os_mock.h"
#include "ifstream_mock.h"
#include "ofstream_mock.h"
#include "job_runner_mock.h"
#include "context_mock.h"
#include "process_mock.h"
#include "session_mock.h"

TEST(build, create_directory_error)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    auto internal_os = std::make_unique<StrictMock<os_mock>>();

    std::filesystem::path job_dir = "/test/test_job";

    EXPECT_CALL(os, list_directory(job_dir))
        .WillOnce(Return(ByMove(
                std::vector<std::unique_ptr<cis1::fs_entry_interface>>{})));

    std::error_code err;
    err.assign(1, err.category());

    EXPECT_CALL(os, create_directory(job_dir / "000000", _))
        .WillOnce(SetArgReferee<1>(err));

    cis1::build build("test_job", job_dir, job_dir / "script", {}, os);

    std::error_code ec;

    build.prepare_build_dir(ec);

    ASSERT_EQ((bool)ec, true);
}

TEST(build, copy_error)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    std::filesystem::path job_dir = "/test/test_job";

    EXPECT_CALL(os, list_directory(job_dir))
        .WillOnce(Return(ByMove(
                std::vector<std::unique_ptr<cis1::fs_entry_interface>>{})));

    EXPECT_CALL(os, create_directory(job_dir / "000000", _))
        .Times(1);

    std::error_code err;
    err.assign(1, err.category());

    EXPECT_CALL(os, copy(
                job_dir / "script",
                job_dir / "000000" / "script",
                _))
        .WillOnce(SetArgReferee<2>(err));

    cis1::build build("test_job", job_dir, job_dir / "script", {}, os);

    std::error_code ec;

    build.prepare_build_dir(ec);

    ASSERT_EQ((bool)ec, true);
}

TEST(build, cant_open_job_params)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    std::filesystem::path job_dir = "/test/test_job";

    EXPECT_CALL(os, list_directory(job_dir))
        .WillOnce(Return(ByMove(
                std::vector<std::unique_ptr<cis1::fs_entry_interface>>{})));

    EXPECT_CALL(os, create_directory(job_dir / "000000", _))
        .Times(1);

    EXPECT_CALL(os, copy(
                job_dir / "script",
                job_dir / "000000" / "script",
                _))
        .Times(1);

    auto ss = std::make_unique<StrictMock<ofstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(false));

    EXPECT_CALL(os, open_ofstream(
                job_dir / "000000" / "job.params",
                _))
        .WillOnce(Return(ByMove(std::move(ss))));

    cis1::build build("test_job", job_dir, job_dir / "script", {}, os);

    std::error_code ec;

    build.prepare_build_dir(ec);

    ASSERT_EQ((bool)ec, true);
}

TEST(build, correct)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    std::filesystem::path job_dir = "/test/test_job";

    std::vector<std::unique_ptr<cis1::fs_entry_interface>> fs_entries;
    auto entry1 = std::make_unique<fs_entry_mock>();

    EXPECT_CALL(*entry1, is_directory())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*entry1, path())
        .WillRepeatedly(Return(job_dir / "000011"));

    fs_entries.push_back(std::move(entry1));

    auto entry2 = std::make_unique<fs_entry_mock>();

    EXPECT_CALL(*entry2, is_directory())
        .WillRepeatedly(Return(false));

    fs_entries.push_back(std::move(entry2));

    EXPECT_CALL(os, list_directory(job_dir))
        .WillOnce(Return(ByMove(
             std::move(fs_entries))));

    EXPECT_CALL(os, create_directory(job_dir / "000012", _))
        .Times(1);

    EXPECT_CALL(os, copy(
                job_dir / "script",
                job_dir / "000012" / "script",
                _))
        .Times(1);

    auto ss = std::make_unique<StrictMock<ofstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    EXPECT_CALL(os, open_ofstream(
                job_dir / "000012" / "job.params",
                _))
        .WillOnce(Return(ByMove(std::move(ss))));

    EXPECT_CALL(os, copy(
                job_dir / "job.conf",
                job_dir / "000012" / "job.conf",
                _))
        .Times(1);

    cis1::build build("test_job", job_dir, job_dir / "script", {}, os);

    std::error_code ec;

    build.prepare_build_dir(ec);

    ASSERT_EQ((bool)ec, false);
}

ACTION_TEMPLATE(SaveArgReferee,
                HAS_1_TEMPLATE_PARAMS(int, k),
                AND_1_VALUE_PARAMS(pointer))
{
    *pointer = &::std::get<k>(args);
}

TEST(build_execute, job_runner)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    StrictMock<context_mock> ctx;

    std::filesystem::path job_dir = "/test/test_job";

    boost::process::environment env{};

    EXPECT_CALL(ctx, env())
        .WillOnce(ReturnRef(env));

    StrictMock<process_mock> process;

    StrictMock<sys_stream_mock> std_in;

    EXPECT_CALL(std_in, close())
        .WillOnce(ReturnRef(std_in));

    StrictMock<sys_stream_mock> std_out;

    boost::process::async_pipe* out_pipe;

    EXPECT_CALL(std_out, less_op(_))
        .WillOnce(
                DoAll(
                        SaveArgReferee<0>(&out_pipe),
                        ReturnRef(std_out)));

    StrictMock<sys_stream_mock> std_err;

    EXPECT_CALL(std_err, close())
        .WillOnce(ReturnRef(std_err));

    EXPECT_CALL(process, std_in())
        .WillOnce(ReturnRef(std_in));

    EXPECT_CALL(process, std_out())
        .WillOnce(ReturnRef(std_out));

    EXPECT_CALL(process, std_err())
        .WillOnce(ReturnRef(std_err));

    EXPECT_CALL(
            process,
            async_system(
                    _,
                    _,
                    _,
                    _,
                    _,
                    _,
                    _,
                    _))
        .Times(1);

    std::string buffer = "first\nsecond";

    std::vector<std::string> expected_result
    {
        {"first"},
        {"second"}
    };

    std::vector<std::string> result;

    boost::asio::io_context io_ctx;

    cis1::job_runner runner(
            io_ctx,
            ctx.env(),
            job_dir / "000012",
            os);

    runner.run_impl(
            "test_script",
            {},
            [&](const std::string& str)
            {
                result.push_back(str);
            },
            process);

    std::error_code ec;

    boost::asio::async_write(
            *out_pipe,
            boost::asio::const_buffer(buffer.data(), buffer.size()),
            [&](std::error_code err, size_t bytes_transferred) mutable
            {
                std::move(*out_pipe).sink().close();
                ec = err;
            });

    io_ctx.run();

    ASSERT_EQ(expected_result, result);
    ASSERT_EQ((bool)ec, false);
}

TEST(build_execute, correct)
{
    using namespace ::testing;

    StrictMock<os_mock> os;

    std::filesystem::path job_dir = "/test/test_job";

    std::vector<std::unique_ptr<cis1::fs_entry_interface>> fs_entries;

    auto entry1 = std::make_unique<fs_entry_mock>();

    EXPECT_CALL(*entry1, is_directory())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*entry1, path())
        .WillRepeatedly(Return(job_dir / "000011"));

    fs_entries.push_back(std::move(entry1));

    auto entry2 = std::make_unique<fs_entry_mock>();

    EXPECT_CALL(*entry2, is_directory())
        .WillRepeatedly(Return(false));

    fs_entries.push_back(std::move(entry2));

    EXPECT_CALL(os, list_directory(job_dir))
        .WillOnce(Return(ByMove(
             std::move(fs_entries))));

    EXPECT_CALL(os, create_directory(job_dir / "000012", _))
        .Times(1);

    EXPECT_CALL(os, copy(
                job_dir / "script",
                job_dir / "000012" / "script",
                _))
        .Times(1);

    auto ss = std::make_unique<StrictMock<ofstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    EXPECT_CALL(os, open_ofstream(
                job_dir / "000012" / "job.params",
                _))
        .WillOnce(Return(ByMove(std::move(ss))));

    EXPECT_CALL(os, copy(
                job_dir / "job.conf",
                job_dir / "000012" / "job.conf",
                _))
        .Times(1);

    StrictMock<context_mock> ctx;

    boost::process::environment env{};

    EXPECT_CALL(ctx, set_env("build", "000012"))
        .Times(1);

    EXPECT_CALL(ctx, env())
        .WillOnce(ReturnRef(env));

    StrictMock<job_runner_factory_mock> job_runner_factory;

    auto job_runner = std::make_unique<job_runner_mock>();

    std::error_code err;

    EXPECT_CALL(
            *job_runner,
            run_impl(
                    "script",
                    _,
                    _))
        .WillOnce(InvokeArgument<1>(err, 0));

    EXPECT_CALL(
            job_runner_factory,
            call_operator(
                    _,
                    _,
                    _,
                    _))
        .WillOnce(Return(ByMove(std::move(job_runner))));

    ss = std::make_unique<StrictMock<ofstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    EXPECT_CALL(os, open_ofstream(
                job_dir / "000012" / "output.txt",
                _))
        .WillOnce(Return(ByMove(std::move(ss))));

    ss = std::make_unique<StrictMock<ofstream_mock>>();

    std::stringstream fc;

    EXPECT_CALL(*ss, ostream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    EXPECT_CALL(
            os,
            open_ofstream(
                    job_dir / "000012" / "exit_code.txt",
                    _))
        .WillOnce(Return(ByMove(std::move(ss))));

    cis1::build build("test_job", job_dir, job_dir / "script", {}, os);

    std::error_code ec;

    build.prepare_build_dir(ec);

    ASSERT_EQ((bool)ec, false);

    int exit_code = -1;

    build.execute(
            ctx,
            ec,
            exit_code,
            std::ref(job_runner_factory));

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ(exit_code, 0);
}

TEST(build_prepare_params, correct)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;

    std::filesystem::path base_dir = "/test";

    std::filesystem::path job_dir = base_dir / "test_job";

    std::string session_id = "test_session";

    EXPECT_CALL(session, session_id())
        .WillOnce(ReturnRef(session_id));

    EXPECT_CALL(ctx, base_dir())
        .WillOnce(ReturnRef(base_dir));

    EXPECT_CALL(
            os,
            exists( base_dir / "sessions" / (session_id + ".prm"),
                    _))
        .WillOnce(Return(true));

    auto ss = std::make_unique<StrictMock<ifstream_mock>>();

    std::stringstream fc;

    fc << "param3=33\n"
       << "param1=11\n";

    EXPECT_CALL(*ss, istream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    EXPECT_CALL(
            os,
            open_ifstream(
                    base_dir / "sessions" / (session_id + ".prm"),
                    _))
        .WillOnce(Return(ByMove(std::move(ss))));

    cis1::build build("test_job", job_dir, job_dir / "script", {}, os);

    auto& params = build.params();

    params["param1"] = "1";
    params["param2"] = "2";
    params["param3"] = "3";

    std::map<std::string, std::string> expected_params =
    {
        {"param1", "11"},
        {"param2", "2"},
        {"param3", "33"}
    };

    std::error_code ec;

    build.prepare_params(ctx, session, ec);

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ(is_maps_equal(params, expected_params), true);
}
