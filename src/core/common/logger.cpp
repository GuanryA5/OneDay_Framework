#include "logger.h"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace oneday {
namespace core {

std::shared_ptr<spdlog::logger> Logger::logger_;

void Logger::initialize() {
    try {
        // 创建控制台输出sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::info);

        // 创建文件输出sink（5MB轮转，3个文件）
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/oneday_framework.log", 1024 * 1024 * 5, 3);
        file_sink->set_level(spdlog::level::debug);

        // 创建多输出logger
        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
        logger_ = std::make_shared<spdlog::logger>("OneDay", sinks.begin(), sinks.end());

        // 设置日志格式
        logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
        logger_->set_level(spdlog::level::debug);

        // 注册为默认logger
        spdlog::register_logger(logger_);
        spdlog::set_default_logger(logger_);

    } catch (const spdlog::spdlog_ex& ex) {
        // 如果日志初始化失败，使用基础控制台输出
        logger_ = spdlog::stdout_color_mt("OneDay");
        logger_->error("Log initialization failed: {}", ex.what());
    }
}

bool Logger::isInitialized() {
    return logger_ != nullptr;
}

void Logger::setLevel(Level level) {
    if (logger_) {
        logger_->set_level(static_cast<spdlog::level::level_enum>(level));
    }
}

void Logger::info(const std::string& message) {
    if (logger_)
        logger_->info(message);
}

void Logger::debug(const std::string& message) {
    if (logger_)
        logger_->debug(message);
}

void Logger::warn(const std::string& message) {
    if (logger_)
        logger_->warn(message);
}

void Logger::warning(const std::string& message) {
    warn(message);  // 调用warn方法
}

void Logger::error(const std::string& message) {
    if (logger_)
        logger_->error(message);
}

void Logger::critical(const std::string& message) {
    if (logger_)
        logger_->critical(message);
}

void Logger::flush() {
    if (logger_)
        logger_->flush();
}

}  // namespace core
}  // namespace oneday