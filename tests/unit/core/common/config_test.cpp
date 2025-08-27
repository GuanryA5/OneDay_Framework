#include <gtest/gtest.h>
#include "core/common/config.h"
#include <filesystem>

using namespace oneday::core;

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_unique<Config>();
        testConfigFile = "test_config.json";
    }
    
    void TearDown() override {
        config.reset();
        if (std::filesystem::exists(testConfigFile)) {
            std::filesystem::remove(testConfigFile);
        }
    }
    
    std::unique_ptr<Config> config;
    std::string testConfigFile;
};

TEST_F(ConfigTest, Initialize) {
    ASSERT_NE(config, nullptr);
}

TEST_F(ConfigTest, DefaultValues) {
    EXPECT_EQ(config->getString("application/name"), "OneDay Framework");
    EXPECT_EQ(config->getString("application/version"), "1.0.0");
    EXPECT_TRUE(config->getBool("application/debug"));
}

TEST_F(ConfigTest, SaveAndLoad) {
    // 保存默认配置
    EXPECT_TRUE(config->save(testConfigFile));
    EXPECT_TRUE(std::filesystem::exists(testConfigFile));
    
    // 创建新配置对象并加载
    auto newConfig = std::make_unique<Config>();
    EXPECT_TRUE(newConfig->load(testConfigFile));
    
    // 验证加载的值
    EXPECT_EQ(newConfig->getString("application/name"), "OneDay Framework");
}

TEST_F(ConfigTest, GetValues) {
    EXPECT_EQ(config->getString("nonexistent", "default"), "default");
    EXPECT_EQ(config->getInt("nonexistent", 42), 42);
    EXPECT_EQ(config->getBool("nonexistent", true), true);
    EXPECT_EQ(config->getDouble("nonexistent", 3.14), 3.14);
}