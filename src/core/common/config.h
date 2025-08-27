#pragma once

#include <string>
#include <memory>

namespace oneday {
namespace core {

/**
 * @brief 配置管理类
 * 基于JSON的应用程序配置管理
 */
class Config {
public:
    Config();
    ~Config();
    
    /**
     * @brief 从文件加载配置
     * @param filename 配置文件路径
     * @return 是否成功
     */
    bool load(const std::string& filename);
    
    /**
     * @brief 保存配置到文件
     * @param filename 配置文件路径
     * @return 是否成功
     */
    bool save(const std::string& filename) const;
    
    /**
     * @brief 获取字符串配置项
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * @brief 获取整数配置项
     */
    int getInt(const std::string& key, int defaultValue = 0) const;
    
    /**
     * @brief 获取布尔配置项
     */
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    /**
     * @brief 获取浮点数配置项
     */
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    
    /**
     * @brief 设置配置项
     */
    template<typename T>
    void set(const std::string& key, const T& value);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace core
} // namespace oneday