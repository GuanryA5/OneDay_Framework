#pragma once

#include <string>
#include <vector>
#include <codecvt>
#include <locale>

#ifdef _WIN32
#include <windows.h>
#endif

namespace oneday::core {

/**
 * @brief 编码转换工具类
 * 处理UTF-8、GBK、Unicode等编码转换问题
 */
class EncodingUtils {
public:
    /**
     * @brief UTF-8字符串转换为宽字符串(UTF-16)
     * @param utf8Str UTF-8编码的字符串
     * @return UTF-16宽字符串
     */
    static std::wstring utf8ToWide(const std::string& utf8Str);
    
    /**
     * @brief 宽字符串(UTF-16)转换为UTF-8字符串
     * @param wideStr UTF-16宽字符串
     * @return UTF-8编码的字符串
     */
    static std::string wideToUtf8(const std::wstring& wideStr);
    
    /**
     * @brief 系统本地编码转换为UTF-8
     * @param localStr 本地编码字符串(Windows下通常是GBK)
     * @return UTF-8编码的字符串
     */
    static std::string localToUtf8(const std::string& localStr);
    
    /**
     * @brief UTF-8转换为系统本地编码
     * @param utf8Str UTF-8编码的字符串
     * @return 本地编码字符串(Windows下通常是GBK)
     */
    static std::string utf8ToLocal(const std::string& utf8Str);
    
    /**
     * @brief 检测字符串是否为有效的UTF-8编码
     * @param str 待检测的字符串
     * @return 是否为有效UTF-8
     */
    static bool isValidUtf8(const std::string& str);
    
    /**
     * @brief 获取UTF-8字符串的实际字符数(不是字节数)
     * @param utf8Str UTF-8编码的字符串
     * @return 字符数
     */
    static size_t utf8Length(const std::string& utf8Str);
    
    /**
     * @brief 截取UTF-8字符串(按字符数，不是字节数)
     * @param utf8Str UTF-8编码的字符串
     * @param start 起始字符位置
     * @param length 字符长度
     * @return 截取后的UTF-8字符串
     */
    static std::string utf8Substr(const std::string& utf8Str, size_t start, size_t length = std::string::npos);
    
    /**
     * @brief 设置控制台输出编码为UTF-8
     * Windows下需要调用此函数以正确显示中文
     */
    static void setConsoleUtf8();
    
    /**
     * @brief 获取系统默认编码页
     * @return 编码页标识符
     */
    static unsigned int getSystemCodePage();

private:
    // 禁用实例化
    EncodingUtils() = delete;
    ~EncodingUtils() = delete;
    EncodingUtils(const EncodingUtils&) = delete;
    EncodingUtils& operator=(const EncodingUtils&) = delete;
};

/**
 * @brief UTF-8字符串字面量宏
 * 确保字符串以UTF-8编码存储
 */
#define U8(str) reinterpret_cast<const char*>(u8##str)

/**
 * @brief 安全的UTF-8字符串类
 * 自动处理编码转换的字符串包装类
 */
class Utf8String {
public:
    Utf8String() = default;
    Utf8String(const char* str);
    Utf8String(const std::string& str);
    Utf8String(const std::wstring& wstr);
    
    // 转换操作符
    operator std::string() const { return data_; }
    operator std::wstring() const { return EncodingUtils::utf8ToWide(data_); }
    
    // 基本操作
    const std::string& str() const { return data_; }
    std::wstring wstr() const { return EncodingUtils::utf8ToWide(data_); }
    const char* c_str() const { return data_.c_str(); }
    
    // 字符串操作
    size_t length() const { return EncodingUtils::utf8Length(data_); }
    size_t size() const { return data_.size(); }  // 字节数
    bool empty() const { return data_.empty(); }
    
    Utf8String substr(size_t start, size_t length = std::string::npos) const {
        return Utf8String(EncodingUtils::utf8Substr(data_, start, length));
    }
    
    // 比较操作
    bool operator==(const Utf8String& other) const { return data_ == other.data_; }
    bool operator!=(const Utf8String& other) const { return data_ != other.data_; }
    bool operator<(const Utf8String& other) const { return data_ < other.data_; }
    
    // 拼接操作
    Utf8String operator+(const Utf8String& other) const {
        return Utf8String(data_ + other.data_);
    }
    
    Utf8String& operator+=(const Utf8String& other) {
        data_ += other.data_;
        return *this;
    }

private:
    std::string data_;
};

} // namespace oneday::core
