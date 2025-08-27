#include <gtest/gtest.h>

#include <iostream>

// 最简单的测试，验证测试框架是否工作
TEST(SimpleTest, BasicTest) {
    std::cout << "Running BasicTest..." << std::endl;
    EXPECT_EQ(1 + 1, 2);
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
    std::cout << "BasicTest completed!" << std::endl;
}

TEST(SimpleTest, StringTest) {
    std::cout << "Running StringTest..." << std::endl;
    std::string hello = "Hello";
    std::string world = "World";
    EXPECT_EQ(hello + " " + world, "Hello World");
    std::cout << "StringTest completed!" << std::endl;
}

// 添加一个 main 函数来确保程序可以独立运行
int main(int argc, char **argv) {
    std::cout << "Starting unit tests..." << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    std::cout << "Unit tests finished with result: " << result << std::endl;
    return result;
}
