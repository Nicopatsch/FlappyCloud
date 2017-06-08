#include "gtest/gtest.h"


int addition(int a, int b) {
    return a+b;
}

TEST(MyFirstTest, TestAddition) {
    EXPECT_EQ(3, addition(1, 2));
}
GTEST_API_ int main(int argc, char **argv) {
    printf("Running main() from gtest_main.cc\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

