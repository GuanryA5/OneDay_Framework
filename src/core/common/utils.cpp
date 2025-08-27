#include "utils.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <random>
#include <regex>
#include <sstream>
#include <thread>

#include "logger.h"

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace oneday::common {

// 基础实现 - 大部分方法返回默认值或简单实现
std::string Utils::getApplicationDataPath() {
    return "./data";
}

std::string Utils::getApplicationConfigPath() {
    return "./config";
}

std::string Utils::getApplicationCachePath() {
    return "./cache";
}

std::string Utils::getApplicationTempPath() {
    return "./temp";
}

std::string Utils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Utils::getCurrentTimestampISO() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

int64_t Utils::getCurrentTimestampMs() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

std::string Utils::formatTimestamp(int64_t timestampMs) {
    auto time_point = std::chrono::system_clock::from_time_t(timestampMs / 1000);
    auto time_t = std::chrono::system_clock::to_time_t(time_point);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Utils::generateUuid() {
    // 简单的UUID生成实现
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 32; ++i) {
        if (i == 8 || i == 12 || i == 16 || i == 20) {
            ss << "-";
        }
        ss << dis(gen);
    }
    return ss.str();
}

std::string Utils::generateShortId(int length) {
    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(chars.size()) - 1);

    std::string result;
    for (int i = 0; i < length; ++i) {
        result += chars[dis(gen)];
    }
    return result;
}

std::string Utils::calculateMD5(const std::string& text) {
    // 简单实现，返回固定值
    return "md5_hash_placeholder";
}

std::string Utils::calculateSHA256(const std::string& text) {
    // 简单实现，返回固定值
    return "sha256_hash_placeholder";
}

std::string Utils::calculateFileMD5(const std::string& filePath) {
    return "file_md5_placeholder";
}

std::string Utils::calculateFileSHA256(const std::string& filePath) {
    return "file_sha256_placeholder";
}

bool Utils::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

bool Utils::directoryExists(const std::string& dirPath) {
    return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
}

bool Utils::createDirectory(const std::string& dirPath) {
    try {
        return std::filesystem::create_directories(dirPath);
    } catch (...) {
        return false;
    }
}

bool Utils::removeFile(const std::string& filePath) {
    try {
        return std::filesystem::remove(filePath);
    } catch (...) {
        return false;
    }
}

bool Utils::removeDirectory(const std::string& dirPath) {
    try {
        return std::filesystem::remove_all(dirPath) > 0;
    } catch (...) {
        return false;
    }
}

bool Utils::copyFile(const std::string& sourcePath, const std::string& destPath) {
    try {
        std::filesystem::copy_file(sourcePath, destPath);
        return true;
    } catch (...) {
        return false;
    }
}

int64_t Utils::getFileSize(const std::string& filePath) {
    try {
        return std::filesystem::file_size(filePath);
    } catch (...) {
        return -1;
    }
}

std::string Utils::getFileExtension(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.extension().string();
}

std::string Utils::getFileName(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.filename().string();
}

std::string Utils::getFileBaseName(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.stem().string();
}

std::string Utils::getFilePath(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.parent_path().string();
}

std::string Utils::formatFileSize(int64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024 && unit < 4) {
        size /= 1024;
        unit++;
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << units[unit];
    return ss.str();
}

std::string Utils::readTextFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Utils::writeTextFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    file << content;
    return file.good();
}

std::map<std::string, std::any> Utils::readJsonFile(const std::string& filePath) {
    // 简单实现，返回空map
    return std::map<std::string, std::any>();
}

bool Utils::writeJsonFile(const std::string& filePath,
                          const std::map<std::string, std::any>& jsonObject) {
    // 简单实现，返回true
    return true;
}

void Utils::sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Utils::sleepSeconds(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

std::string Utils::getSystemInfo() {
    return "System info placeholder";
}

int Utils::getRandomInt(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

double Utils::getRandomDouble(double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

std::string Utils::escapeHtml(const std::string& text) {
    std::string result = text;
    // 简单的HTML转义实现
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    pos = 0;
    while ((pos = result.find("<", pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    return result;
}

std::string Utils::unescapeHtml(const std::string& text) {
    std::string result = text;
    // 简单的HTML反转义实现
    size_t pos = 0;
    while ((pos = result.find("&amp;", pos)) != std::string::npos) {
        result.replace(pos, 5, "&");
        pos += 1;
    }
    pos = 0;
    while ((pos = result.find("&lt;", pos)) != std::string::npos) {
        result.replace(pos, 4, "<");
        pos += 1;
    }
    pos = 0;
    while ((pos = result.find("&gt;", pos)) != std::string::npos) {
        result.replace(pos, 4, ">");
        pos += 1;
    }
    return result;
}

std::vector<std::string> Utils::splitString(const std::string& text, const std::string& separator) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = text.find(separator);

    while (end != std::string::npos) {
        result.push_back(text.substr(start, end - start));
        start = end + separator.length();
        end = text.find(separator, start);
    }
    result.push_back(text.substr(start));

    return result;
}

std::string Utils::joinStrings(const std::vector<std::string>& strings,
                               const std::string& separator) {
    if (strings.empty()) {
        return "";
    }

    std::stringstream ss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) {
            ss << separator;
        }
        ss << strings[i];
    }
    return ss.str();
}

std::string Utils::trimString(const std::string& text) {
    size_t start = text.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }

    size_t end = text.find_last_not_of(" \t\n\r");
    return text.substr(start, end - start + 1);
}

bool Utils::isValidEmail(const std::string& email) {
    std::regex email_regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, email_regex);
}

bool Utils::isValidUrl(const std::string& url) {
    std::regex url_regex(R"(https?://[^\s/$.?#].[^\s]*)");
    return std::regex_match(url, url_regex);
}

}  // namespace oneday::common
