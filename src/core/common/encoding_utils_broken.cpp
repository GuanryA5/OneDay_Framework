#include "encoding_utils.h"
#include <codecvt>
#include <locale>
#include <algorithm>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#endif

namespace oneday::core {

// UTF-8 编码验证
bool EncodingUtils::isValidUtf8(const std::string& str) {
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.c_str());
    size_t len = str.length();
    
    for (size_t i = 0; i < len; ) {
        unsigned char byte = bytes[i];
        
        if (byte <= 0x7F) {
            // ASCII 字符
            i++;
        } else if ((byte & 0xE0) == 0xC0) {
            // 2字节UTF-8字符
            if (i + 1 >= len || (bytes[i + 1] & 0xC0) != 0x80) {
                return false;
            }
            i += 2;
        } else if ((byte & 0xF0) == 0xE0) {
            // 3字节UTF-8字符
            if (i + 2 >= len || 
                (bytes[i + 1] & 0xC0) != 0x80 || 
                (bytes[i + 2] & 0xC0) != 0x80) {
                return false;
            }
            i += 3;
        } else if ((byte & 0xF8) == 0xF0) {
            // 4字节UTF-8字符
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

// 本地编码转UTF-8
std::string EncodingUtils::localToUtf8(const std::string& localStr) {
    if (isValidUtf8(localStr)) {
        return localStr;
    }
    
#ifdef _WIN32
    try {
        // 首先转换为宽字符
        int wideSize = MultiByteToWideChar(CP_ACP, 0, localStr.c_str(), -1, nullptr, 0);
        if (wideSize > 0) {
            std::wstring wideStr(wideSize - 1, L'\0');
            MultiByteToWideChar(CP_ACP, 0, localStr.c_str(), -1, &wideStr[0], wideSize);
            return wideToUtf8(wideStr);
        }
    } catch (const std::exception&) {
        // 转换失败，返回原字符串
    }
#endif
    
    return localStr;
}

// UTF-8转本地编码
std::string EncodingUtils::utf8ToLocal(const std::string& utf8Str) {
    if (!isValidUtf8(utf8Str)) {
        return utf8Str;
    }
    
#ifdef _WIN32
    try {
        // UTF-8转宽字符
        std::wstring wideStr = utf8ToWide(utf8Str);
        
        // 宽字符转本地编码
        int localSize = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (localSize > 0) {
            std::string localStr(localSize - 1, '\0');
            WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, &localStr[0], localSize, nullptr, nullptr);
            return localStr;
        }
    } catch (const std::exception&) {
        // 转换失败，返回原字符串
    }
#endif
    
    return utf8Str;
}

// 宽字符转UTF-8
std::string EncodingUtils::wideToUtf8(const std::wstring& wideStr) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wideStr);
    } catch (const std::exception&) {
        return "";
    }
}

// UTF-8转宽字符
std::wstring EncodingUtils::utf8ToWide(const std::string& utf8Str) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(utf8Str);
    } catch (const std::exception&) {
        return L"";
    }
}

// 字符串清理
std::string EncodingUtils::cleanString(const std::string& str) {
    std::string result = str;
    
    // 移除控制字符（除了换行符和制表符）
    result.erase(std::remove_if(result.begin(), result.end(), 
        [](unsigned char c) {
            return (c < 32 && c != '\n' && c != '\t' && c != '\r') || c == 127;
        }), result.end());
    
    return result;
}

// 字符串截断（按字符数，不是字节数）
std::string EncodingUtils::truncateUtf8(const std::string& utf8Str, size_t maxChars) {
    if (!isValidUtf8(utf8Str)) {
        // 如果不是有效的UTF-8，按字节截断
        if (utf8Str.length() <= maxChars) {
            return utf8Str;
        }
        return utf8Str.substr(0, maxChars);
    }
    
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(utf8Str.c_str());
    size_t len = utf8Str.length();
    size_t charCount = 0;
    size_t byteIndex = 0;
    
    while (byteIndex < len && charCount < maxChars) {
        unsigned char byte = bytes[byteIndex];
        
        if (byte <= 0x7F) {
            byteIndex++;
        } else if ((byte & 0xE0) == 0xC0) {
            byteIndex += 2;
        } else if ((byte & 0xF0) == 0xE0) {
            byteIndex += 3;
        } else if ((byte & 0xF8) == 0xF0) {
            byteIndex += 4;
        } else {
            // 无效字符，跳过
            byteIndex++;
        }
        
        charCount++;
    }
    
    return utf8Str.substr(0, byteIndex);
}

// 获取UTF-8字符串的字符数
size_t EncodingUtils::getUtf8CharCount(const std::string& utf8Str) {
    if (!isValidUtf8(utf8Str)) {
        return utf8Str.length();
    }
    
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(utf8Str.c_str());
    size_t len = utf8Str.length();
    size_t charCount = 0;
    
    for (size_t i = 0; i < len; ) {
        unsigned char byte = bytes[i];
        
        if (byte <= 0x7F) {
            i++;
        } else if ((byte & 0xE0) == 0xC0) {
            i += 2;
        } else if ((byte & 0xF0) == 0xE0) {
            i += 3;
        } else if ((byte & 0xF8) == 0xF0) {
            i += 4;
        } else {
            i++;
        }
        
        charCount++;
    }
    
    return charCount;
}

// UTF-8字符串子串（按字符索引）
std::string EncodingUtils::utf8Substr(const std::string& utf8Str, size_t start, size_t length) {
    if (!isValidUtf8(utf8Str)) {
        // 如果不是有效的UTF-8，按字节处理
        if (start >= utf8Str.length()) {
            return "";
        }
        size_t endPos = std::min(start + length, utf8Str.length());
        return utf8Str.substr(start, endPos - start);
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

        if (byte <= 0x7F) {
            i++;
        } else if ((byte & 0xE0) == 0xC0) {
            i += 2;
        } else if ((byte & 0xF0) == 0xE0) {
            i += 3;
        } else if ((byte & 0xF8) == 0xF0) {
            i += 4;
        } else {
            i++;
        }
    }

    // 如果起始位置超出范围
    if (charIndex < start) {
        return "";
    }

    // 找到结束字节位置
    for (size_t i = startByte; i < len && charIndex < start + length; ++charIndex) {
        endByte = i;
        unsigned char byte = bytes[i];

        if (byte <= 0x7F) {
            i++;
        } else if ((byte & 0xE0) == 0xC0) {
            i += 2;
        } else if ((byte & 0xF0) == 0xE0) {
            i += 3;
        } else if ((byte & 0xF8) == 0xF0) {
            i += 4;
        } else {
            i++;
        }
    }

    return utf8Str.substr(startByte, endByte - startByte);
}

std::string EncodingUtils::normalizeLineEndings(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\r') {
            if (i + 1 < str.length() && str[i + 1] == '\n') {
                // Windows风格 \r\n -> \n
                result += '\n';
                ++i; // 跳过 \n
            } else {
                // Mac风格 \r -> \n
                result += '\n';
            }
        } else {
            result += str[i];
        }
    }
    
    return result;
}

// Utf8String 类实现
Utf8String::Utf8String(const std::string& str) : data_(str) {
    if (!EncodingUtils::isValidUtf8(data_)) {
        data_ = EncodingUtils::localToUtf8(data_);
    }
}

Utf8String::Utf8String(const char* str) : data_(str ? str : "") {
    if (!EncodingUtils::isValidUtf8(data_)) {
        data_ = EncodingUtils::localToUtf8(data_);
    }
}

Utf8String::Utf8String(const std::wstring& wstr) {
    data_ = EncodingUtils::wideToUtf8(wstr);
}

}  // namespace oneday::core
