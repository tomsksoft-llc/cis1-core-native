#include <gtest/gtest.h>

#include "read_istream_kv_str.h"
#include "test_utils.h"

TEST(read_istream_kv_str, encoded_values)
{
    std::stringstream ss;
    ss << R"(test\\\=key=test\nvalue)" << '\n';
    ss << R"(testkey=test\=value)";

    std::map<std::string, std::string> lines;

    std::map<std::string, std::string> expected_lines
    {
            {R"(test\=key)", "test\nvalue"},
            {"testkey", "test=value"}
    };

    std::error_code ec;
    const auto decode = true;
    cis1::read_istream_kv_str(ss, lines, ec, decode);
    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ(is_maps_equal(lines, expected_lines), true);
}

TEST(read_istream_kv_str, empty_line)
{
    std::stringstream ss;
    ss << R"(test\\\=key=test\nvalue)" << '\n';
    ss << R"(test\=\=key=test value)" << "\r\n";
    ss << '\n';
    ss << R"(testkey\\=test\=value)";

    std::map<std::string, std::string> lines;

    std::map<std::string, std::string> expected_lines
            {
                    {R"(test\=key)", "test\nvalue"},
                    {R"(test==key)", "test value"},
                    {R"(testkey\)", "test=value"}
            };

    std::error_code ec;
    const auto decode = true;
    cis1::read_istream_kv_str(ss, lines, ec, decode);
    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ(is_maps_equal(lines, expected_lines), true);
}

TEST(read_istream_kv_str, empty_file)
{
    std::stringstream ss;
    std::map<std::string, std::string> lines;

    std::error_code ec;

    cis1::read_istream_kv_str(ss, lines, ec);

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ(lines.empty(), true);
}

TEST(read_istream_kv_str, trailing_space)
{
    std::stringstream ss;
    ss << "a=b\r\n";
    ss << "b=c\n";
    ss << "c=d\r\n";

    std::map<std::string, std::string> lines;

    std::map<std::string, std::string> lines2
    {
        {"a", "b"},
        {"b", "c"},
        {"c", "d"}
    };

    std::error_code ec;

    cis1::read_istream_kv_str(ss, lines, ec);

    ASSERT_EQ((bool)ec, false);
    ASSERT_EQ(is_maps_equal(lines, lines2), true);
}

TEST(read_istream_kv_str, empty_value)
{
    std::stringstream ss;
    ss << "a=\n";
    ss << "b=\n";

    std::map<std::string, std::string> lines;

    std::map<std::string, std::string> lines2
    {
        {"a", ""},
        {"b", ""}
    };

    std::error_code ec;

    cis1::read_istream_kv_str(ss, lines, ec);

    ASSERT_NE((bool)ec, true);
    ASSERT_EQ(is_maps_equal(lines, lines2), true);
}

TEST(read_istream_kv_str, invalid_file)
{
    std::stringstream ss;
    ss << "=a";

    std::map<std::string, std::string> lines;

    std::error_code ec;
    cis1::read_istream_kv_str(ss, lines, ec);
    ASSERT_EQ((bool)ec, true);
    ASSERT_EQ(lines.empty(), true);
}
