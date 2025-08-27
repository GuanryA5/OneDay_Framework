#include "encoding_utils.h"

#include <algorithm>
#include <stdexcept>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#endif

namespace oneday::core {

std::wstring EncodingUtils::utf8ToWide(const std::string& utf8Str) {
    if (utf8Str.empty()) {
        return std::wstring();
    }

#ifdef _WIN32
    // Windows API方式转换
    int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    if (wideSize <= 0) {
        throw std::runtime_error("UTF-8 to wide string conversion failed");
    }

    std::wstring wideStr(wideSize - 1, 0);  // -1 排除null终止符
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideSize);
    return wideStr;
#else
    // 使用标准库转换
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(utf8Str);
#endif
}

std::string EncodingUtils::wideToUtf8(const std::wstring& wideStr) {
    if (wideStr.empty()) {
        return std::string();
    }

#ifdef _WIN32
    // Windows API方式转换
    int utf8Size =
        WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8Size <= 0) {
        throw std::runtime_error("Wide string to UTF-8 conversion failed");
    }

    std::string utf8Str(utf8Size - 1, 0);  // -1 排除null终止符
    WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], utf8Size, nullptr, nullptr);
    return utf8Str;
#else
    // 使用标准库转换
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wideStr);
#endif
}

std::string EncodingUtils::localToUtf8(const std::string& localStr) {
    if (localStr.empty()) {
        return std::string();
    }

#ifdef _WIN32
    // 先转换为宽字符，再转换为UTF-8
    int wideSize = MultiByteToWideChar(CP_ACP, 0, localStr.c_str(), -1, nullptr, 0);
    if (wideSize <= 0) {
        return localStr;  // 转换失败，返回原字符串
    }

    std::wstring wideStr(wideSize - 1, 0);
    MultiByteToWideChar(CP_ACP, 0, localStr.c_str(), -1, &wideStr[0], wideSize);

    return wideToUtf8(wideStr);
#else
    // Linux下假设本地编码就是UTF-8
    return localStr;
#endif
}

std::string EncodingUtils::utf8ToLocal(const std::string& utf8Str) {
    if (utf8Str.empty()) {
        return std::string();
    }

#ifdef _WIN32
    // 先转换为宽字符，再转换为本地编码
    std::wstring wideStr = utf8ToWide(utf8Str);

    int localSize =
        WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (localSize <= 0) {
        return utf8Str;  // 转换失败，返回原字符串
    }

    std::string localStr(localSize - 1, 0);
    WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, &localStr[0], localSize, nullptr, nullptr);

    return localStr;
#else
    // Linux下假设本地编码就是UTF-8
    return utf8Str;
#endif
}

bool EncodingUtils::isValidUtf8(const std::string& str) {
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.c_str());
    size_t len = str.length();

    for (size_t i = 0; i < len; ++i) {
        unsigned char byte = bytes[i];

        if (byte < 0x80) {
            // ASCII字符
            continue;
        } else if ((byte >> 5) == 0x06) {
            // 110xxxxx - 2字节序列
            if (++i >= len || (bytes[i] & 0xC0) != 0x80)
                return false;
        } else if ((byte >> 4) == 0x0E) {
            // 1110xxxx - 3字节序列
            if (++i >= len || (bytes[i] & 0xC0) != 0x80)
                return false;
            if (++i >= len || (bytes[i] & 0xC0) != 0x80)
                return false;
        } else if ((byte >> 3) == 0x1E) {
            // 11110xxx - 4字节序列
            if (++i >= len || (bytes[i] & 0xC0) != 0x80)
                return false;
            if (++i >= len || (bytes[i] & 0xC0) != 0x80)
                return false;
            if (++i >= len || (bytes[i] & 0xC0) != 0x80)
                return false;
        } else {
            return false;
        }
    }

    return true;
}

size_t EncodingUtils::utf8Length(const std::string& utf8Str) {
    size_t length = 0;
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(utf8Str.c_str());
    size_t len = utf8Str.length();

    for (size_t i = 0; i < len; ++length) {
        unsigned char byte = bytes[i];

        if (byte < 0x80) {
            i += 1;  // ASCII字符
        } else if ((byte >> 5) == 0x06) {
            i += 2;  // 2字节UTF-8字符
        } else if ((byte >> 4) == 0x0E) {
            i += 3;  // 3字节UTF-8字符
        } else if ((byte >> 3) == 0x1E) {
            i += 4;  // 4字节UTF-8字符
        } else {
            i += 1;  // 无效字符，跳过
        }
    }

    return length;
}

std::string EncodingUtils::utf8Substr(const std::string& utf8Str, size_t start, size_t length) {
    if (utf8Str.empty() || start >= utf8Length(utf8Str)) {
        return std::string();
    }

    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(utf8Str.c_str());
    size_t len = utf8Str.length();
    size_t charIndex = 0;
    size_t startByte = 0;
    size_t endByte = len;

    // 找到起始字节位置
    for (size_t i = 0; i < len && charIndex < start; ++charIndex) {
        startByte = i;
        unsigned char byte = bytes[i];

        if (byte < 0x80) {
            i += 1;
        } else if ((byte >> 5) == 0x06) {
            i += 2;
        } else if ((byte >> 4) == 0x0E) {
            i += 3;
        } else if ((byte >> 3) == 0x1E) {
            i += 4;
        } else {
            i += 1;
        }
    }

    // 如果指定了长度，找到结束字节位置
    if (length != std::string::npos) {
        size_t endChar = start + length;
        for (size_t i = startByte; i < len && charIndex < endChar; ++charIndex) {
            unsigned char byte = bytes[i];

            if (byte < 0x80) {
                i += 1;
            } else if ((byte >> 5) == 0x06) {
                i += 2;
            } else if ((byte >> 4) == 0x0E) {
                i += 3;
            } else if ((byte >> 3) == 0x1E) {
                i += 4;
            } else {
                i += 1;
            }

            if (charIndex + 1 == endChar) {
                endByte = i;
                break;
            }
        }
    }

    return utf8Str.substr(startByte, endByte - startByte);
}

void EncodingUtils::setConsoleUtf8() {
#ifdef _WIN32
    // 设置控制台输入输出编码为UTF-8
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // 设置C运行时库的locale
    setlocale(LC_ALL, ".UTF8");

    // 设置标准输入输出流为UTF-8模式
    _setmode(_fileno(stdin), _O_U8TEXT);
    _setmode(_fileno(stdout), _O_U8TEXT);
    _setmode(_fileno(stderr), _O_U8TEXT);
#endif
}

unsigned int EncodingUtils::getSystemCodePage() {
#ifdef _WIN32
    return GetACP();
#else
    return 65001;  // UTF-8
#endif
}

// Utf8String实现
Utf8String::Utf8String(const char* str) : data_(str ? str : "") {
    // 如果不是有效的UTF-8，尝试从本地编码转换
    if (!EncodingUtils::isValidUtf8(data_)) {
        data_ = EncodingUtils::localToUtf8(data_);
    }
}

Utf8String::Utf8String(const std::string& str) : data_(str) {
    // 如果不是有效的UTF-8，尝试从本地编码转换
    if (!EncodingUtils::isValidUtf8(data_)) {
        data_ = EncodingUtils::localToUtf8(data_);
    }
}

Utf8String::Utf8String(const std::wstring& wstr) {
    data_ = EncodingUtils::wideToUtf8(wstr);
}
