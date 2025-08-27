#include "core/common/encoding_utils.h"

#include <gtest/gtest.h>

#include <chrono>
#include <iostream>

using namespace oneday::core;

class EncodingUtilsTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // 测试用的中文字符串
        utf8Chinese = "你好世界";
        utf8Mixed = "Hello世界123";
        utf8Empty = "";

        // 对应的宽字符串
        wideChinese = L"你好世界";
        wideMixed = L"Hello世界123";
        wideEmpty = L"";
    }

    std::string utf8Chinese;
    std::string utf8Mixed;
    std::string utf8Empty;

    std::wstring wideChinese;
    std::wstring wideMixed;
    std::wstring wideEmpty;
};

// 测试UTF-8到宽字符转换
TEST_F(EncodingUtilsTest, Utf8ToWideConversion) {
    // 测试中文字符串
    std::wstring result = EncodingUtils::utf8ToWide(utf8Chinese);
    EXPECT_EQ(result, wideChinese);

    // 测试混合字符串
    result = EncodingUtils::utf8ToWide(utf8Mixed);
    EXPECT_EQ(result, wideMixed);

    // 测试空字符串
    result = EncodingUtils::utf8ToWide(utf8Empty);
    EXPECT_EQ(result, wideEmpty);
}

// 测试宽字符到UTF-8转换
TEST_F(EncodingUtilsTest, WideToUtf8Conversion) {
    // 测试中文字符串
    std::string result = EncodingUtils::wideToUtf8(wideChinese);
    EXPECT_EQ(result, utf8Chinese);

    // 测试混合字符串
    result = EncodingUtils::wideToUtf8(wideMixed);
    EXPECT_EQ(result, utf8Mixed);

    // 测试空字符串
    result = EncodingUtils::wideToUtf8(wideEmpty);
    EXPECT_EQ(result, utf8Empty);
}

// 测试往返转换
TEST_F(EncodingUtilsTest, RoundTripConversion) {
    // UTF-8 -> Wide -> UTF-8
    std::string result = EncodingUtils::wideToUtf8(EncodingUtils::utf8ToWide(utf8Chinese));
    EXPECT_EQ(result, utf8Chinese);

    // Wide -> UTF-8 -> Wide
    std::wstring wideResult = EncodingUtils::utf8ToWide(EncodingUtils::wideToUtf8(wideChinese));
    EXPECT_EQ(wideResult, wideChinese);
}

// 测试UTF-8有效性检查
TEST_F(EncodingUtilsTest, Utf8Validation) {
    // 有效的UTF-8字符串
    EXPECT_TRUE(EncodingUtils::isValidUtf8("Hello"));
    EXPECT_TRUE(EncodingUtils::isValidUtf8("你好"));
    EXPECT_TRUE(EncodingUtils::isValidUtf8("Hello世界"));
    EXPECT_TRUE(EncodingUtils::isValidUtf8(""));

    // 无效的UTF-8字节序列
    std::string invalid1 = "\xFF\xFE";  // 无效起始字节
    std::string invalid2 = "\xC0\x80";  // 过长编码
    EXPECT_FALSE(EncodingUtils::isValidUtf8(invalid1));
    EXPECT_FALSE(EncodingUtils::isValidUtf8(invalid2));
}

// 测试UTF-8字符长度计算
TEST_F(EncodingUtilsTest, Utf8Length) {
    // ASCII字符
    EXPECT_EQ(EncodingUtils::utf8Length("Hello"), 5);

    // 中文字符（每个字符3字节）
    EXPECT_EQ(EncodingUtils::utf8Length("你好"), 2);

    // 混合字符
    EXPECT_EQ(EncodingUtils::utf8Length("Hello世界"), 7);

    // 空字符串
    EXPECT_EQ(EncodingUtils::utf8Length(""), 0);

    // 验证与字节长度的区别
    std::string chinese = "你好";
    EXPECT_EQ(chinese.length(), 6);                    // 字节数
    EXPECT_EQ(EncodingUtils::utf8Length(chinese), 2);  // 字符数
}

// 测试UTF-8字符串截取
TEST_F(EncodingUtilsTest, Utf8Substr) {
    std::string mixed = "Hello世界123";

    // 从开始截取
    EXPECT_EQ(EncodingUtils::utf8Substr(mixed, 0, 5), "Hello");
    EXPECT_EQ(EncodingUtils::utf8Substr(mixed, 5, 2), "世界");
    EXPECT_EQ(EncodingUtils::utf8Substr(mixed, 7, 3), "123");

    // 截取到末尾
    EXPECT_EQ(EncodingUtils::utf8Substr(mixed, 5), "世界123");

    // 边界情况
    EXPECT_EQ(EncodingUtils::utf8Substr(mixed, 0, 0), "");
    EXPECT_EQ(EncodingUtils::utf8Substr(mixed, 100, 5), "");  // 超出范围
    EXPECT_EQ(EncodingUtils::utf8Substr("", 0, 5), "");       // 空字符串
}

// 测试Utf8String类
TEST_F(EncodingUtilsTest, Utf8StringClass) {
    // 构造函数测试
    Utf8String str1("你好");
    Utf8String str2(std::string("世界"));
    Utf8String str3(L"Hello");

    // 基本操作
    EXPECT_EQ(str1.length(), 2);
    EXPECT_EQ(str1.str(), "你好");
    EXPECT_FALSE(str1.empty());

    // 字符串拼接
    Utf8String combined = str1 + str2;
    EXPECT_EQ(combined.str(), "你好世界");

    // 字符串截取
    Utf8String substr = combined.substr(0, 2);
    EXPECT_EQ(substr.str(), "你好");

    // 比较操作
    Utf8String str4("你好");
    EXPECT_TRUE(str1 == str4);
    EXPECT_FALSE(str1 == str2);

    // 转换操作
    std::wstring wide = str1.wstr();
    EXPECT_EQ(wide, L"你好");
}

// 测试系统编码页获取
TEST_F(EncodingUtilsTest, SystemCodePage) {
    unsigned int codePage = EncodingUtils::getSystemCodePage();

#ifdef _WIN32
    // Windows下应该返回有效的代码页
    EXPECT_GT(codePage, 0);
#else
    // Linux下应该返回UTF-8的代码页
    EXPECT_EQ(codePage, 65001);
#endif
}

// 性能测试（可选）
TEST_F(EncodingUtilsTest, PerformanceTest) {
    const int iterations = 1000;
    std::string testStr = "这是一个性能测试字符串，包含中文和English混合内容";

    // 测试转换性能
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        std::wstring wide = EncodingUtils::utf8ToWide(testStr);
        std::string back = EncodingUtils::wideToUtf8(wide);
        EXPECT_EQ(back, testStr);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // 输出性能信息（仅在调试时）
    std::cout << "转换性能测试: " << iterations << " 次迭代耗时 " << duration.count() << " 微秒"
              << std::endl;

    // 确保性能在合理范围内（每次转换小于100微秒）
    EXPECT_LT(duration.count(), iterations * 100);
}

// 边界条件测试
TEST_F(EncodingUtilsTest, EdgeCases) {
    // 测试非常长的字符串
    std::string longStr(10000, 'A');
    std::wstring longWide = EncodingUtils::utf8ToWide(longStr);
    EXPECT_EQ(longWide.length(), 10000);

    // 测试包含特殊字符的字符串
    std::string special = "特殊字符：\n\t\r\"'\\";
    std::wstring specialWide = EncodingUtils::utf8ToWide(special);
    std::string backToUtf8 = EncodingUtils::wideToUtf8(specialWide);
    EXPECT_EQ(backToUtf8, special);

    // 测试Unicode各个平面的字符
    std::string emoji = "😀🌍🚀";  // Emoji字符
    if (EncodingUtils::isValidUtf8(emoji)) {
        std::wstring emojiWide = EncodingUtils::utf8ToWide(emoji);
        std::string emojiBack = EncodingUtils::wideToUtf8(emojiWide);
        EXPECT_EQ(emojiBack, emoji);
    }
}
