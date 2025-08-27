#include "encoding_utils.h"
#include <codecvt>
#include <locale>
#include <algorithm>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#endif

namespace oneday::core {

// UTF-8转宽字符
std::wstring EncodingUtils::utf8ToWide(const std::string& utf8Str) {
    if (utf8Str.empty()) {
        return std::wstring();
    }

#ifdef _WIN32
    // Windows API方式转换
    int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    if (wideSize > 0) {
        std::wstring wideStr(wideSize - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideSize);
        return wideStr;
    }
#else
    // 使用标准库转换
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(utf8Str);
    } catch (const std::exception&) {
        return L"";
    }
#endif
    
    return std::wstring();
}

// 宽字符转UTF-8
std::string EncodingUtils::wideToUtf8(const std::wstring& wideStr) {
    if (wideStr.empty()) {
        return std::string();
    }

#ifdef _WIN32
    // Windows API方式转换
    int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8Size > 0) {
        std::string utf8Str(utf8Size - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], utf8Size, nullptr, nullptr);
        return utf8Str;
    }
#else
    // 使用标准库转换
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wideStr);
    } catch (const std::exception&) {
        return "";
    }
#endif
    
    return std::string();
}

// 本地编码转UTF-8
std::string EncodingUtils::localToUtf8(const std::string& localStr) {
    if (localStr.empty()) {
        return std::string();
    }

#ifdef _WIN32
    // 首先转换为宽字符
    int wideSize = MultiByteToWideChar(CP_ACP, 0, localStr.c_str(), -1, nullptr, 0);
    if (wideSize > 0) {
        std::wstring wideStr(wideSize - 1, L'\0');
        MultiByteToWideChar(CP_ACP, 0, localStr.c_str(), -1, &wideStr[0], wideSize);
        return wideToUtf8(wideStr);
    }
#endif
    
    // 如果转换失败或非Windows系统，返回原字符串
    return localStr;
}

// UTF-8转本地编码
std::string EncodingUtils::utf8ToLocal(const std::string& utf8Str) {
    if (utf8Str.empty()) {
        return std::string();
    }

#ifdef _WIN32
    // UTF-8转宽字符
    std::wstring wideStr = utf8ToWide(utf8Str);
    if (wideStr.empty()) {
        return utf8Str;
    }
    
    // 宽字符转本地编码
    int localSize = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (localSize > 0) {
        std::string localStr(localSize - 1, '\0');
        WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, &localStr[0], localSize, nullptr, nullptr);
        return localStr;
    }
#endif
    
    // 如果转换失败或非Windows系统，返回原字符串
    return utf8Str;
}

// 检测是否为有效UTF-8
bool EncodingUtils::isValidUtf8(const std::string& str) {
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.c_str());
    size_t len = str.length();
    
    for (size_t i = 0; i < len; ) {
        unsigned char byte = bytes[i];
        
        if (byte < 0x80) {
            // ASCII字符
            i += 1;
        } else if ((byte >> 5) == 0x06) {
            // 110xxxxx - 2字节序列
            if (i + 1 >= len || (bytes[i + 1] & 0xC0) != 0x80) {
                return false;
            }
            i += 2;
        } else if ((byte >> 4) == 0x0E) {
            // 1110xxxx - 3字节序列
            if (i + 2 >= len || 
                (bytes[i + 1] & 0xC0) != 0x80 || 
                (bytes[i + 2] & 0xC0) != 0x80) {
                return false;
            }
            i += 3;
        } else if ((byte >> 3) == 0x1E) {
            // 11110xxx - 4字节序列
            if (i + 3 >= len || 
                (bytes[i + 1] & 0xC0) != 0x80 || 
                (bytes[i + 2] & 0xC0) != 0x80 || 
                (bytes[i + 3] & 0xC0) != 0x80) {
                return false;
            }
            i += 4;
        } else {
            return false;
        }
    }
    
    return true;
}

// 获取UTF-8字符串长度
size_t EncodingUtils::utf8Length(const std::string& utf8Str) {
    if (!isValidUtf8(utf8Str)) {
        return utf8Str.length();
    }
    
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(utf8Str.c_str());
    size_t len = utf8Str.length();
    size_t charCount = 0;
    
    for (size_t i = 0; i < len; ) {
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
        
        charCount++;
    }
    
    return charCount;
}

// UTF-8字符串子串
std::string EncodingUtils::utf8Substr(const std::string& utf8Str, size_t start, size_t length) {
    if (!isValidUtf8(utf8Str)) {
        // 如果不是有效的UTF-8，按字节处理
        if (start >= utf8Str.length()) {
            return "";
        }
        if (length == std::string::npos) {
            return utf8Str.substr(start);
        }
        return utf8Str.substr(start, length);
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

}  // namespace oneday::core
