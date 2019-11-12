#include <gtest/gtest.h>

#include "job.h"
#include "os_mock.h"
#include "context_mock.h"
#include "error_code.h"
#include "ifstream_mock.h"
#include "ofstream_mock.h"
#include "utils.h"
#include "session_mock.h"
#include "fs_entry_mock.h"
#include "process_mock.h"
#include "job_runner_mock.h"

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

TEST(job, create_directory_error)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillRepeatedly(ReturnRef(base_dir));

    std::filesystem::path job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, list_directory(job_dir))
        .WillOnce(Return(ByMove(
                std::vector<std::unique_ptr<cis1::fs_entry_interface>>{})));

    std::error_code err;
    err.assign(1, err.category());

    EXPECT_CALL(os, create_directory(job_dir / "000000", _))
        .WillOnce(DoAll(SetArgReferee<1>(err), Return(false)));

    cis1::job job(
            "test_job",
            {
                "test_script",
                5,
                5,
                {}
            },
            {}, {}, {},
            os);

    std::error_code ec;

    job.prepare_build(ctx, session, {}, ec);

    ASSERT_EQ((bool)ec, true);
}

TEST(job, copy_error)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillRepeatedly(ReturnRef(base_dir));

    std::filesystem::path job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, list_directory(job_dir))
        .WillOnce(Return(ByMove(
                std::vector<std::unique_ptr<cis1::fs_entry_interface>>{})));

    EXPECT_CALL(os, create_directory(job_dir / "000000", _))
        .WillOnce(Return(true));

    std::error_code err;
    err.assign(1, err.category());

    EXPECT_CALL(os, copy(
                job_dir / "test_script",
                job_dir / "000000" / "test_script",
                _))
        .WillOnce(SetArgReferee<2>(err));

    cis1::job job(
            "test_job",
            {
                "test_script",
                5,
                5,
                {}
            },
            {}, {}, {},
            os);

    std::error_code ec;

    job.prepare_build(ctx, session, {}, ec);

    ASSERT_EQ((bool)ec, true);
}

TEST(job, cant_open_job_params)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillRepeatedly(ReturnRef(base_dir));

    std::filesystem::path job_dir = base_dir / "jobs" / "test_job";

    EXPECT_CALL(os, list_directory(job_dir))
        .WillOnce(Return(ByMove(
                std::vector<std::unique_ptr<cis1::fs_entry_interface>>{})));

    EXPECT_CALL(os, create_directory(job_dir / "000000", _))
        .WillOnce(Return(true));

    EXPECT_CALL(os, copy(
                job_dir / "test_script",
                job_dir / "000000" / "test_script",
                _))
        .Times(1);

    auto ss = std::make_unique<StrictMock<ofstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(false));

    EXPECT_CALL(os, open_ofstream(
                job_dir / "000000" / "job.params",
                _))
        .WillOnce(Return(ByMove(std::move(ss))));

    cis1::job job(
            "test_job",
            {
                "test_script",
                5,
                5,
                {}
            },
            {}, {}, {},
            os);

    std::error_code ec;

    job.prepare_build(ctx, session, {}, ec);

    ASSERT_EQ((bool)ec, true);
}

TEST(job, cant_open_session_id)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillRepeatedly(ReturnRef(base_dir));

    std::filesystem::path job_dir = base_dir / "jobs" / "test_job";

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
        .WillOnce(Return(true));

    EXPECT_CALL(os, copy(
                job_dir / "test_script",
                job_dir / "000012" / "test_script",
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

    ss = std::make_unique<StrictMock<ofstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(false));

    EXPECT_CALL(os, open_ofstream(
                job_dir / "000012" / "session_id.txt",
                _))
        .WillOnce(Return(ByMove(std::move(ss))));

    cis1::job job(
            "test_job",
            {
                "test_script",
                5,
                5,
                {}
            },
            {}, {}, {},
            os);

    std::error_code ec;

    job.prepare_build(ctx, session, {}, ec);

    ASSERT_EQ((bool)ec, true);
}

TEST(job, correct)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillRepeatedly(ReturnRef(base_dir));

    std::filesystem::path job_dir = base_dir / "jobs" / "test_job";

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
        .WillOnce(Return(true));

    EXPECT_CALL(os, copy(
                job_dir / "test_script",
                job_dir / "000012" / "test_script",
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

    ss = std::make_unique<StrictMock<ofstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc;

    EXPECT_CALL(*ss, ostream())
        .WillOnce(ReturnRef(fc));

    EXPECT_CALL(os, open_ofstream(
                job_dir / "000012" / "session_id.txt",
                _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::string session_id = "test_session";

    EXPECT_CALL(session, session_id())
        .WillOnce(ReturnRef(session_id));

    cis1::job job(
            "test_job",
            {
                "test_script",
                5,
                5,
                {}
            },
            {}, {}, {},
            os);

    std::error_code ec;

    job.prepare_build(ctx, session, {}, ec);

    ASSERT_EQ((bool)ec, false);
    ASSERT_STREQ(fc.str().c_str(), (session_id + "\n").c_str());
}

ACTION_TEMPLATE(SaveArgReferee,
                HAS_1_TEMPLATE_PARAMS(int, k),
                AND_1_VALUE_PARAMS(pointer))
{
    *pointer = &::std::get<k>(args);
}

TEST(job_execute, job_runner)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillRepeatedly(ReturnRef(base_dir));

    std::filesystem::path job_dir = base_dir / "jobs" / "test_job";

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

    boost::process::async_pipe* err_pipe;

    EXPECT_CALL(std_err, less_op(_))
        .WillOnce(
                DoAll(
                        SaveArgReferee<0>(&err_pipe),
                        ReturnRef(std_err)));

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
                std::move(*err_pipe).sink().close();
                ec = err;
            });

    io_ctx.run();

    ASSERT_EQ(expected_result, result);
    ASSERT_EQ((bool)ec, false);
}

TEST(job_execute, correct)
{
    using namespace ::testing;

    StrictMock<os_mock> os;
    StrictMock<context_mock> ctx;
    StrictMock<session_mock> session;

    std::filesystem::path base_dir = "test_base_dir";

    EXPECT_CALL(ctx, base_dir())
        .WillRepeatedly(ReturnRef(base_dir));

    std::filesystem::path job_dir = base_dir / "jobs" / "test_job";

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
        .WillOnce(Return(true));

    EXPECT_CALL(os, copy(
                job_dir / "test_script",
                job_dir / "000012" / "test_script",
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
                    "test_script",
                    _,
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
                    job_dir / "000012" / "exitcode.txt",
                    _))
        .WillOnce(Return(ByMove(std::move(ss))));

    std::string session_id = "test_session";

    EXPECT_CALL(session, session_id())
        .WillOnce(ReturnRef(session_id));

    ss = std::make_unique<StrictMock<ofstream_mock>>();

    EXPECT_CALL(*ss, is_open())
        .WillOnce(Return(true));

    std::stringstream fc2;

    EXPECT_CALL(*ss, ostream())
        .WillOnce(ReturnRef(fc2));

    EXPECT_CALL(os, open_ofstream(
                job_dir / "000012" / "session_id.txt",
                _))
        .WillOnce(Return(ByMove(std::move(ss))));

    cis1::job job(
            "test_job",
            {
                "test_script",
                5,
                5,
                {}
            },
            {}, {}, {},
            os);

    std::error_code ec;

    job.prepare_build(ctx, session, {}, ec);

    ASSERT_EQ((bool)ec, false);

    int exit_code = -1;

    job.execute(
            12,
            ctx,
            ec,
            exit_code,
            std::ref(job_runner_factory));

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ(exit_code, 0);
    ASSERT_STREQ(fc2.str().c_str(), (session_id + "\n").c_str());
}
