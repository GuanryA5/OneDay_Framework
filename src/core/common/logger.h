#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace oneday {
namespace core {

/**
 * @brief 日志系统
 * 基于spdlog的高性能日志记录
 */
class Logger {
  public:
    enum class Level { Trace = 0, Debug = 1, Info = 2, Warn = 3, Error = 4, Critical = 5 };

    /**
     * @brief 初始化日志系统
     */
    static void initialize();

    /**
     * @brief 检查日志系统是否已初始化
     */
    static bool isInitialized();

    /**
     * @brief 设置日志级别
     */
    static void setLevel(Level level);

    /**
     * @brief 记录信息日志
     */
    static void info(const std::string& message);

    /**
     * @brief 记录调试日志
     */
    static void debug(const std::string& message);

    /**
     * @brief 记录警告日志
     */
    static void warn(const std::string& message);

    /**
     * @brief 记录警告信息（别名）
     */
    static void warning(const std::string& message);

    /**
     * @brief 记录错误日志
     */
    static void error(const std::string& message);

    /**
     * @brief 记录严重错误日志
     */
    static void critical(const std::string& message);

    /**
     * @brief 刷新日志缓冲区
     */
    static void flush();

  private:
    Logger() = default;
    static std::shared_ptr<spdlog::logger> logger_;
};

}  // namespace core
}  // namespace oneday