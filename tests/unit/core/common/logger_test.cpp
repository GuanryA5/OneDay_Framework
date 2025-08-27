#include <gtest/gtest.h>
#include "core/common/logger.h"

using namespace oneday::core;

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::initialize();
    }
};

TEST_F(LoggerTest, Initialize) {
    EXPECT_NO_THROW(Logger::initialize());
}

TEST_F(LoggerTest, LogMessages) {
    EXPECT_NO_THROW(Logger::info("Test info message"));
    EXPECT_NO_THROW(Logger::debug("Test debug message"));
    EXPECT_NO_THROW(Logger::warn("Test warning message"));
    EXPECT_NO_THROW(Logger::error("Test error message"));
    EXPECT_NO_THROW(Logger::critical("Test critical message"));
}

TEST_F(LoggerTest, SetLevel) {
    EXPECT_NO_THROW(Logger::setLevel(Logger::Level::Info));
    EXPECT_NO_THROW(Logger::setLevel(Logger::Level::Debug));
    EXPECT_NO_THROW(Logger::setLevel(Logger::Level::Error));
}

TEST_F(LoggerTest, Flush) {
    Logger::info("Test message before flush");
    EXPECT_NO_THROW(Logger::flush());
}