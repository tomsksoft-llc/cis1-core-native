#include <gtest/gtest.h>
#include "cis1_core.h"

TEST(example_test_case, first_test)
{
    ASSERT_NE(1, 2);
}

TEST(example_test_case, second_case_test)
{
    ASSERT_NE(1, 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
