#include "config.h"
#include "logger.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

namespace oneday {
namespace core {

class Config::Impl {
public:
    nlohmann::json configData;
    std::string currentFile;
    
    // 默认配置
    void setDefaults() {
        configData["application"] = {
            {"name", "OneDay Framework"},
            {"version", "1.0.0"},
            {"debug", true}
        };
        
        configData["logging"] = {
            {"level", "debug"},
            {"max_file_size", "5MB"},
            {"max_files", 3}
        };
        
        configData["blueprint"] = {
            {"max_nodes", 500},
            {"execution_timeout", 30000}
        };
        
        configData["ui"] = {
            {"theme", "dark"},
            {"auto_save", true},
            {"auto_save_interval", 300}
        };
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->setDefaults();
    Logger::debug("Config initialized with default values");
}

Config::~Config() = default;

bool Config::load(const std::string& filename) {
    try {
        if (!std::filesystem::exists(filename)) {
            Logger::warn("Config file not found, creating with defaults: " + filename);
            save(filename);
            return true;
        }
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            Logger::error("Cannot open config file: " + filename);
            return false;
        }
        
        file >> pImpl->configData;
        pImpl->currentFile = filename;
        
        Logger::info("Configuration loaded from: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        Logger::error("Failed to load config: " + std::string(e.what()));
        return false;
    }
}

bool Config::save(const std::string& filename) const {
    try {
        // 确保目录存在
        auto dir = std::filesystem::path(filename).parent_path();
        if (!dir.empty()) {
            std::filesystem::create_directories(dir);
        }
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            Logger::error("Cannot write config file: " + filename);
            return false;
        }
        
        file << pImpl->configData.dump(2);
        Logger::info("Configuration saved to: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        Logger::error("Failed to save config: " + std::string(e.what()));
        return false;
    }
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    try {
        return pImpl->configData.value(key, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

int Config::getInt(const std::string& key, int defaultValue) const {
    try {
        return pImpl->configData.value(key, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    try {
        return pImpl->configData.value(key, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    try {
        return pImpl->configData.value(key, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

template<typename T>
void Config::set(const std::string& key, const T& value) {
    pImpl->configData[key] = value;
}

// 显式模板实例化
template void Config::set<std::string>(const std::string&, const std::string&);
template void Config::set<int>(const std::string&, const int&);
template void Config::set<bool>(const std::string&, const bool&);
template void Config::set<double>(const std::string&, const double&);

} // namespace core
} // namespace oneday