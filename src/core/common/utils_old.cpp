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

// 其他方法的简单实现
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
    std::uniform_int_distribution<> dis(0, chars.size() - 1);

    std::string result;
    for (int i = 0; i < length; ++i) {
        result += chars[dis(gen)];
    }
    return result;
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

void Utils::sleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Utils::sleepSeconds(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
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

}  // namespace oneday::common

QString Utils::getApplicationCachePath() {
    QString cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir dir(cachePath);
    if (!dir.exists()) {
        dir.mkpath(cachePath);
    }
    return cachePath;
}

QString Utils::getApplicationTempPath() {
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString appTempPath = QDir(tempPath).absoluteFilePath(QCoreApplication::applicationName());
    QDir dir(appTempPath);
    if (!dir.exists()) {
        dir.mkpath(appTempPath);
    }
    return appTempPath;
}

QString Utils::getCurrentTimestamp() {
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

QString Utils::getCurrentTimestampISO() {
    return QDateTime::currentDateTime().toString(Qt::ISODate);
}

qint64 Utils::getCurrentTimestampMs() {
    return QDateTime::currentMSecsSinceEpoch();
}

QString Utils::formatTimestamp(qint64 timestampMs) {
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timestampMs);
    return dateTime.toString("yyyy-MM-dd hh:mm:ss");
}

QString Utils::generateUuid() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString Utils::generateShortId(int length) {
    const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QString result;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, chars.length() - 1);

    for (int i = 0; i < length; ++i) {
        result += chars[dis(gen)];
    }

    return result;
}

QString Utils::calculateMD5(const QString& text) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(text.toUtf8());
    return hash.result().toHex();
}

QString Utils::calculateSHA256(const QString& text) {
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(text.toUtf8());
    return hash.result().toHex();
}

QString Utils::calculateFileMD5(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        Logger::error("Failed to open file for MD5 calculation: " + filePath);
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(&file);
    return hash.result().toHex();
}

QString Utils::calculateFileSHA256(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        Logger::error("Failed to open file for SHA256 calculation: " + filePath);
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(&file);
    return hash.result().toHex();
}

bool Utils::fileExists(const QString& filePath) {
    return QFileInfo::exists(filePath);
}

bool Utils::directoryExists(const QString& dirPath) {
    return QDir(dirPath).exists();
}

bool Utils::createDirectory(const QString& dirPath) {
    QDir dir;
    bool success = dir.mkpath(dirPath);
    if (success) {
        Logger::info("Directory created: " + dirPath);
    } else {
        Logger::error("Failed to create directory: " + dirPath);
    }
    return success;
}

bool Utils::removeFile(const QString& filePath) {
    bool success = QFile::remove(filePath);
    if (success) {
        Logger::info("File removed: " + filePath);
    } else {
        Logger::error("Failed to remove file: " + filePath);
    }
    return success;
}

bool Utils::removeDirectory(const QString& dirPath) {
    QDir dir(dirPath);
    bool success = dir.removeRecursively();
    if (success) {
        Logger::info("Directory removed: " + dirPath);
    } else {
        Logger::error("Failed to remove directory: " + dirPath);
    }
    return success;
}

bool Utils::copyFile(const QString& sourcePath, const QString& destPath) {
    // 如果目标文件存在，先删除
    if (QFile::exists(destPath)) {
        QFile::remove(destPath);
    }

    bool success = QFile::copy(sourcePath, destPath);
    if (success) {
        Logger::info("File copied from " + sourcePath + " to " + destPath);
    } else {
        Logger::error("Failed to copy file from " + sourcePath + " to " + destPath);
    }
    return success;
}

qint64 Utils::getFileSize(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    return fileInfo.size();
}

QString Utils::getFileExtension(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    return fileInfo.suffix();
}

QString Utils::getFileName(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    return fileInfo.fileName();
}

QString Utils::getFileBaseName(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    return fileInfo.baseName();
}

QString Utils::getFilePath(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    return fileInfo.path();
}

QString Utils::formatFileSize(qint64 bytes) {
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    const qint64 TB = GB * 1024;

    if (bytes >= TB) {
        return QString::number(bytes / TB, 'f', 2) + " TB";
    } else if (bytes >= GB) {
        return QString::number(bytes / GB, 'f', 2) + " GB";
    } else if (bytes >= MB) {
        return QString::number(bytes / MB, 'f', 2) + " MB";
    } else if (bytes >= KB) {
        return QString::number(bytes / KB, 'f', 2) + " KB";
    } else {
        return QString::number(bytes) + " B";
    }
}

QString Utils::readTextFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Logger::error("Failed to open file for reading: " + filePath);
        return QString();
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    return in.readAll();
}

bool Utils::writeTextFile(const QString& filePath, const QString& content) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        Logger::error("Failed to open file for writing: " + filePath);
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << content;

    Logger::info("Text file written: " + filePath);
    return true;
}

QJsonObject Utils::readJsonFile(const QString& filePath) {
    QString content = readTextFile(filePath);
    if (content.isEmpty()) {
        return QJsonObject();
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        Logger::error("JSON parse error in file " + filePath + ": " + error.errorString());
        return QJsonObject();
    }

    return doc.object();
}

bool Utils::writeJsonFile(const QString& filePath, const QJsonObject& jsonObject) {
    QJsonDocument doc(jsonObject);
    QString content = doc.toJson(QJsonDocument::Indented);
    return writeTextFile(filePath, content);
}

void Utils::sleep(int milliseconds) {
    QThread::msleep(milliseconds);
}

void Utils::sleepSeconds(int seconds) {
    QThread::sleep(seconds);
}

QString Utils::getSystemInfo() {
    QJsonObject info;
    info["os"] = QSysInfo::prettyProductName();
    info["kernel"] = QSysInfo::kernelType() + " " + QSysInfo::kernelVersion();
    info["architecture"] = QSysInfo::currentCpuArchitecture();
    info["hostname"] = QSysInfo::machineHostName();
    info["application"] = QCoreApplication::applicationName();
    info["version"] = QCoreApplication::applicationVersion();
    info["qt_version"] = QT_VERSION_STR;

    QJsonDocument doc(info);
    return doc.toJson(QJsonDocument::Compact);
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

QString Utils::escapeHtml(const QString& text) {
    QString escaped = text;
    escaped.replace("&", "&amp;");
    escaped.replace("<", "&lt;");
    escaped.replace(">", "&gt;");
    escaped.replace("\"", "&quot;");
    escaped.replace("'", "&#39;");
    return escaped;
}

QString Utils::unescapeHtml(const QString& text) {
    QString unescaped = text;
    unescaped.replace("&amp;", "&");
    unescaped.replace("&lt;", "<");
    unescaped.replace("&gt;", ">");
    unescaped.replace("&quot;", "\"");
    unescaped.replace("&#39;", "'");
    return unescaped;
}

QStringList Utils::splitString(const QString& text, const QString& separator) {
    return text.split(separator, Qt::SkipEmptyParts);
}

QString Utils::joinStrings(const QStringList& strings, const QString& separator) {
    return strings.join(separator);
}

QString Utils::trimString(const QString& text) {
    return text.trimmed();
}

bool Utils::isValidEmail(const QString& email) {
    QRegularExpression regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return regex.match(email).hasMatch();
}

bool Utils::isValidUrl(const QString& url) {
    QUrl qurl(url);
    return qurl.isValid() && !qurl.scheme().isEmpty();
}

}  // namespace oneday::common
