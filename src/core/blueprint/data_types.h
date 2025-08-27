#pragma once

#include <string>
#include <variant>
#include <vector>
#include <map>
#include <memory>
#include <typeinfo>
#include <functional>

namespace oneday {
namespace core {
namespace blueprint {

/**
 * @brief 蓝图系统支持的数据类型枚举
 */
enum class DataType {
    None,           ///< 无类型/空类型
    Boolean,        ///< 布尔类型
    Integer,        ///< 整数类型
    Float,          ///< 浮点数类型
    String,         ///< 字符串类型
    Vector2,        ///< 二维向量
    Vector3,        ///< 三维向量
    Color,          ///< 颜色类型
    Object,         ///< 对象引用
    Array,          ///< 数组类型
    Execution       ///< 执行流类型
};

/**
 * @brief 二维向量结构
 */
struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;
    
    Vector2() = default;
    Vector2(float x_, float y_) : x(x_), y(y_) {}
    
    bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }
};

/**
 * @brief 三维向量结构
 */
struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    
    Vector3() = default;
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    
    bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

/**
 * @brief 颜色结构
 */
struct Color {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
    
    Color() = default;
    Color(float r_, float g_, float b_, float a_ = 1.0f) : r(r_), g(g_), b(b_), a(a_) {}
    
    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};

/**
 * @brief 执行流标记（用于控制流节点）
 */
struct ExecutionToken {
    bool valid = true;
    
    ExecutionToken() = default;
    explicit ExecutionToken(bool v) : valid(v) {}
    
    bool operator==(const ExecutionToken& other) const {
        return valid == other.valid;
    }
};

// 前向声明
class BlueprintValue;

/**
 * @brief 数组类型定义
 */
using BlueprintArray = std::vector<BlueprintValue>;

/**
 * @brief 对象引用类型
 */
using ObjectReference = std::shared_ptr<void>;

/**
 * @brief 蓝图值的变体类型
 */
using ValueVariant = std::variant<
    std::monostate,     // None
    bool,               // Boolean
    int,                // Integer
    float,              // Float
    std::string,        // String
    Vector2,            // Vector2
    Vector3,            // Vector3
    Color,              // Color
    ObjectReference,    // Object
    BlueprintArray,     // Array
    ExecutionToken      // Execution
>;

/**
 * @brief 蓝图系统的通用值类型
 */
class BlueprintValue {
public:
    /**
     * @brief 默认构造函数
     */
    BlueprintValue();
    
    /**
     * @brief 从具体类型构造
     */
    template<typename T>
    BlueprintValue(const T& value);
    
    /**
     * @brief 拷贝构造函数
     */
    BlueprintValue(const BlueprintValue& other);
    
    /**
     * @brief 移动构造函数
     */
    BlueprintValue(BlueprintValue&& other) noexcept;
    
    /**
     * @brief 赋值操作符
     */
    BlueprintValue& operator=(const BlueprintValue& other);
    
    /**
     * @brief 移动赋值操作符
     */
    BlueprintValue& operator=(BlueprintValue&& other) noexcept;
    
    /**
     * @brief 获取数据类型
     */
    DataType getType() const;
    
    /**
     * @brief 检查是否为指定类型
     */
    template<typename T>
    bool is() const;
    
    /**
     * @brief 获取指定类型的值
     */
    template<typename T>
    T get() const;
    
    /**
     * @brief 尝试获取指定类型的值
     */
    template<typename T>
    bool tryGet(T& outValue) const;
    
    /**
     * @brief 设置值
     */
    template<typename T>
    void set(const T& value);
    
    /**
     * @brief 检查是否为空
     */
    bool isEmpty() const;
    
    /**
     * @brief 清空值
     */
    void clear();
    
    /**
     * @brief 转换为字符串表示
     */
    std::string toString() const;
    
    /**
     * @brief 比较操作符
     */
    bool operator==(const BlueprintValue& other) const;
    bool operator!=(const BlueprintValue& other) const;

private:
    ValueVariant m_value;
    DataType m_type;
    
    void updateType();
};

/**
 * @brief 数据类型工具类
 */
class DataTypeUtils {
public:
    /**
     * @brief 获取数据类型的字符串名称
     */
    static std::string getTypeName(DataType type);
    
    /**
     * @brief 检查两个类型是否兼容
     */
    static bool areTypesCompatible(DataType from, DataType to);
    
    /**
     * @brief 尝试类型转换
     */
    static bool tryConvert(const BlueprintValue& from, DataType toType, BlueprintValue& result);
    
    /**
     * @brief 获取类型的默认值
     */
    static BlueprintValue getDefaultValue(DataType type);
    
    /**
     * @brief 检查类型是否为数值类型
     */
    static bool isNumericType(DataType type);
    
    /**
     * @brief 检查类型是否为向量类型
     */
    static bool isVectorType(DataType type);
    
    /**
     * @brief 获取类型的颜色（用于UI显示）
     */
    static Color getTypeColor(DataType type);

private:
    static const std::map<DataType, std::string> s_typeNames;
    static const std::map<DataType, Color> s_typeColors;
};

/**
 * @brief 类型转换器接口
 */
class TypeConverter {
public:
    virtual ~TypeConverter() = default;
    
    /**
     * @brief 执行类型转换
     */
    virtual bool convert(const BlueprintValue& from, BlueprintValue& to) = 0;
    
    /**
     * @brief 获取源类型
     */
    virtual DataType getFromType() const = 0;
    
    /**
     * @brief 获取目标类型
     */
    virtual DataType getToType() const = 0;
};

/**
 * @brief 类型转换管理器
 */
class TypeConversionManager {
public:
    /**
     * @brief 获取单例实例
     */
    static TypeConversionManager& instance();
    
    /**
     * @brief 注册类型转换器
     */
    void registerConverter(std::unique_ptr<TypeConverter> converter);
    
    /**
     * @brief 执行类型转换
     */
    bool convert(const BlueprintValue& from, DataType toType, BlueprintValue& result);
    
    /**
     * @brief 检查是否支持转换
     */
    bool canConvert(DataType from, DataType to) const;

private:
    TypeConversionManager() = default;
    
    std::map<std::pair<DataType, DataType>, std::unique_ptr<TypeConverter>> m_converters;
    
    void initializeDefaultConverters();
};

} // namespace blueprint
} // namespace core
} // namespace oneday
