#pragma once

#include <any>
#include <map>
#include <string>
#include <vector>

namespace oneday::common {

/**
 * @brief 通用工具类
 *
 * 提供各种常用的工具函数
 */
class Utils {
  public:
    /**
     * @brief 构造函数
     */
    Utils();

    /**
     * @brief 析构函数
     */
    ~Utils();

    // === 路径相关 ===

    /**
     * @brief 获取应用程序数据目录
     * @return 数据目录路径
     */
    static std::string getApplicationDataPath();

    /**
     * @brief 获取应用程序配置目录
     * @return 配置目录路径
     */
    static std::string getApplicationConfigPath();

    /**
     * @brief 获取应用程序缓存目录
     * @return 缓存目录路径
     */
    static std::string getApplicationCachePath();

    /**
     * @brief 获取应用程序临时目录
     * @return 临时目录路径
     */
    static std::string getApplicationTempPath();

    // === 时间相关 ===

    /**
     * @brief 获取当前时间戳字符串
     * @return 时间戳字符串 (yyyy-MM-dd hh:mm:ss)
     */
    static std::string getCurrentTimestamp();

    /**
     * @brief 获取当前ISO格式时间戳
     * @return ISO格式时间戳
     */
    static std::string getCurrentTimestampISO();

    /**
     * @brief 获取当前时间戳（毫秒）
     * @return 毫秒时间戳
     */
    static int64_t getCurrentTimestampMs();

    /**
     * @brief 格式化时间戳
     * @param timestampMs 毫秒时间戳
     * @return 格式化的时间字符串
     */
    static std::string formatTimestamp(int64_t timestampMs);

    // === ID生成 ===

    /**
     * @brief 生成UUID
     * @return UUID字符串
     */
    static std::string generateUuid();

    /**
     * @brief 生成短ID
     * @param length ID长度
     * @return 短ID字符串
     */
    static std::string generateShortId(int length = 8);

    // === 哈希计算 ===

    /**
     * @brief 计算文本的MD5哈希
     * @param text 输入文本
     * @return MD5哈希值
     */
    static std::string calculateMD5(const std::string& text);

    /**
     * @brief 计算文本的SHA256哈希
     * @param text 输入文本
     * @return SHA256哈希值
     */
    static std::string calculateSHA256(const std::string& text);

    /**
     * @brief 计算文件的MD5哈希
     * @param filePath 文件路径
     * @return MD5哈希值
     */
    static std::string calculateFileMD5(const std::string& filePath);

    /**
     * @brief 计算文件的SHA256哈希
     * @param filePath 文件路径
     * @return SHA256哈希值
     */
    static std::string calculateFileSHA256(const std::string& filePath);

    // === 文件操作 ===

    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    static bool fileExists(const std::string& filePath);

    /**
     * @brief 检查目录是否存在
     * @param dirPath 目录路径
     * @return 是否存在
     */
    static bool directoryExists(const std::string& dirPath);

    /**
     * @brief 创建目录
     * @param dirPath 目录路径
     * @return 是否创建成功
     */
    static bool createDirectory(const std::string& dirPath);

    /**
     * @brief 删除文件
     * @param filePath 文件路径
     * @return 是否删除成功
     */
    static bool removeFile(const std::string& filePath);

    /**
     * @brief 删除目录
     * @param dirPath 目录路径
     * @return 是否删除成功
     */
    static bool removeDirectory(const std::string& dirPath);

    /**
     * @brief 复制文件
     * @param sourcePath 源文件路径
     * @param destPath 目标文件路径
     * @return 是否复制成功
     */
    static bool copyFile(const std::string& sourcePath, const std::string& destPath);

    /**
     * @brief 获取文件大小
     * @param filePath 文件路径
     * @return 文件大小（字节）
     */
    static int64_t getFileSize(const std::string& filePath);

    /**
     * @brief 获取文件扩展名
     * @param filePath 文件路径
     * @return 扩展名
     */
    static std::string getFileExtension(const std::string& filePath);

    /**
     * @brief 获取文件名
     * @param filePath 文件路径
     * @return 文件名
     */
    static std::string getFileName(const std::string& filePath);

    /**
     * @brief 获取文件基本名（不含扩展名）
     * @param filePath 文件路径
     * @return 基本名
     */
    static std::string getFileBaseName(const std::string& filePath);

    /**
     * @brief 获取文件所在目录
     * @param filePath 文件路径
     * @return 目录路径
     */
    static std::string getFilePath(const std::string& filePath);

    /**
     * @brief 格式化文件大小
     * @param bytes 字节数
     * @return 格式化的大小字符串
     */
    static std::string formatFileSize(int64_t bytes);

    // === 文件读写 ===

    /**
     * @brief 读取文本文件
     * @param filePath 文件路径
     * @return 文件内容
     */
    static std::string readTextFile(const std::string& filePath);

    /**
     * @brief 写入文本文件
     * @param filePath 文件路径
     * @param content 文件内容
     * @return 是否写入成功
     */
    static bool writeTextFile(const std::string& filePath, const std::string& content);

    /**
     * @brief 读取JSON文件
     * @param filePath 文件路径
     * @return JSON对象
     */
    static std::map<std::string, std::any> readJsonFile(const std::string& filePath);

    /**
     * @brief 写入JSON文件
     * @param filePath 文件路径
     * @param jsonObject JSON对象
     * @return 是否写入成功
     */
    static bool writeJsonFile(const std::string& filePath,
                              const std::map<std::string, std::any>& jsonObject);

    // === 系统相关 ===

    /**
     * @brief 休眠（毫秒）
     * @param milliseconds 毫秒数
     */
    static void sleep(int milliseconds);

    /**
     * @brief 休眠（秒）
     * @param seconds 秒数
     */
    static void sleepSeconds(int seconds);

    /**
     * @brief 获取系统信息
     * @return 系统信息JSON字符串
     */
    static std::string getSystemInfo();

    // === 随机数生成 ===

    /**
     * @brief 生成随机整数
     * @param min 最小值
     * @param max 最大值
     * @return 随机整数
     */
    static int getRandomInt(int min, int max);

    /**
     * @brief 生成随机浮点数
     * @param min 最小值
     * @param max 最大值
     * @return 随机浮点数
     */
    static double getRandomDouble(double min = 0.0, double max = 1.0);

    // === 字符串处理 ===

    /**
     * @brief HTML转义
     * @param text 原始文本
     * @return 转义后的文本
     */
    static std::string escapeHtml(const std::string& text);

    /**
     * @brief HTML反转义
     * @param text 转义的文本
     * @return 原始文本
     */
    static std::string unescapeHtml(const std::string& text);

    /**
     * @brief 分割字符串
     * @param text 原始字符串
     * @param separator 分隔符
     * @return 字符串列表
     */
    static std::vector<std::string> splitString(const std::string& text,
                                                const std::string& separator);

    /**
     * @brief 连接字符串
     * @param strings 字符串列表
     * @param separator 分隔符
     * @return 连接后的字符串
     */
    static std::string joinStrings(const std::vector<std::string>& strings,
                                   const std::string& separator);

    /**
     * @brief 去除字符串首尾空白
     * @param text 原始字符串
     * @return 处理后的字符串
     */
    static std::string trimString(const std::string& text);

    // === 验证函数 ===

    /**
     * @brief 验证邮箱格式
     * @param email 邮箱地址
     * @return 是否有效
     */
    static bool isValidEmail(const std::string& email);

    /**
     * @brief 验证URL格式
     * @param url URL地址
     * @return 是否有效
     */
    static bool isValidUrl(const std::string& url);
};

}  // namespace oneday::common
