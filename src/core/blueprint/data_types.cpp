#include "data_types.h"

#include <iomanip>
#include <sstream>

#include "../common/logger.h"

using oneday::core::Logger;

namespace oneday {
namespace core {
namespace blueprint {

// BlueprintValue 实现

BlueprintValue::BlueprintValue() : m_value(std::monostate{}), m_type(DataType::None) {}

template <typename T>
BlueprintValue::BlueprintValue(const T& value) : m_value(value) {
    updateType();
}

BlueprintValue::BlueprintValue(const BlueprintValue& other)
    : m_value(other.m_value), m_type(other.m_type) {}

BlueprintValue::BlueprintValue(BlueprintValue&& other) noexcept
    : m_value(std::move(other.m_value)), m_type(other.m_type) {
    other.m_type = DataType::None;
}

BlueprintValue& BlueprintValue::operator=(const BlueprintValue& other) {
    if (this != &other) {
        m_value = other.m_value;
        m_type = other.m_type;
    }
    return *this;
}

BlueprintValue& BlueprintValue::operator=(BlueprintValue&& other) noexcept {
    if (this != &other) {
        m_value = std::move(other.m_value);
        m_type = other.m_type;
        other.m_type = DataType::None;
    }
    return *this;
}

DataType BlueprintValue::getType() const {
    return m_type;
}

template <typename T>
bool BlueprintValue::is() const {
    return std::holds_alternative<T>(m_value);
}

template <typename T>
T BlueprintValue::get() const {
    try {
        return std::get<T>(m_value);
    } catch (const std::bad_variant_access& e) {
        Logger::error("BlueprintValue::get() - Type mismatch: " + std::string(e.what()));
        return T{};
    }
}

template <typename T>
bool BlueprintValue::tryGet(T& outValue) const {
    if (std::holds_alternative<T>(m_value)) {
        outValue = std::get<T>(m_value);
        return true;
    }
    return false;
}

template <typename T>
void BlueprintValue::set(const T& value) {
    m_value = value;
    updateType();
}

bool BlueprintValue::isEmpty() const {
    return m_type == DataType::None;
}

void BlueprintValue::clear() {
    m_value = std::monostate{};
    m_type = DataType::None;
}

std::string BlueprintValue::toString() const {
    std::ostringstream oss;

    switch (m_type) {
        case DataType::None:
            return "None";
        case DataType::Boolean:
            return get<bool>() ? "true" : "false";
        case DataType::Integer:
            return std::to_string(get<int>());
        case DataType::Float:
            oss << std::fixed << std::setprecision(3) << get<float>();
            return oss.str();
        case DataType::String:
            return get<std::string>();
        case DataType::Vector2: {
            auto v = get<Vector2>();
            oss << "(" << v.x << ", " << v.y << ")";
            return oss.str();
        }
        case DataType::Vector3: {
            auto v = get<Vector3>();
            oss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
            return oss.str();
        }
        case DataType::Color: {
            auto c = get<Color>();
            oss << "RGBA(" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << ")";
            return oss.str();
        }
        case DataType::Object:
            return "Object";
        case DataType::Array: {
            auto arr = get<BlueprintArray>();
            oss << "Array[" << arr.size() << "]";
            return oss.str();
        }
        case DataType::Execution:
            return get<ExecutionToken>().valid ? "Execute" : "Invalid";
        default:
            return "Unknown";
    }
}

bool BlueprintValue::operator==(const BlueprintValue& other) const {
    if (m_type != other.m_type) {
        return false;
    }

    return m_value == other.m_value;
}

bool BlueprintValue::operator!=(const BlueprintValue& other) const {
    return !(*this == other);
}

void BlueprintValue::updateType() {
    if (std::holds_alternative<std::monostate>(m_value)) {
        m_type = DataType::None;
    } else if (std::holds_alternative<bool>(m_value)) {
        m_type = DataType::Boolean;
    } else if (std::holds_alternative<int>(m_value)) {
        m_type = DataType::Integer;
    } else if (std::holds_alternative<float>(m_value)) {
        m_type = DataType::Float;
    } else if (std::holds_alternative<std::string>(m_value)) {
        m_type = DataType::String;
    } else if (std::holds_alternative<Vector2>(m_value)) {
        m_type = DataType::Vector2;
    } else if (std::holds_alternative<Vector3>(m_value)) {
        m_type = DataType::Vector3;
    } else if (std::holds_alternative<Color>(m_value)) {
        m_type = DataType::Color;
    } else if (std::holds_alternative<ObjectReference>(m_value)) {
        m_type = DataType::Object;
    } else if (std::holds_alternative<BlueprintArray>(m_value)) {
        m_type = DataType::Array;
    } else if (std::holds_alternative<ExecutionToken>(m_value)) {
        m_type = DataType::Execution;
    } else {
        m_type = DataType::None;
    }
}

// DataTypeUtils 实现

const std::map<DataType, std::string> DataTypeUtils::s_typeNames = {
    {DataType::None, "None"},
    {DataType::Boolean, "Boolean"},
    {DataType::Integer, "Integer"},
    {DataType::Float, "Float"},
    {DataType::String, "String"},
    {DataType::Vector2, "Vector2"},
    {DataType::Vector3, "Vector3"},
    {DataType::Color, "Color"},
    {DataType::Object, "Object"},
    {DataType::Array, "Array"},
    {DataType::Execution, "Execution"}};

const std::map<DataType, Color> DataTypeUtils::s_typeColors = {
    {DataType::None, Color(0.5f, 0.5f, 0.5f)},
    {DataType::Boolean, Color(1.0f, 0.0f, 0.0f)},
    {DataType::Integer, Color(0.0f, 1.0f, 0.0f)},
    {DataType::Float, Color(0.0f, 0.8f, 0.0f)},
    {DataType::String, Color(1.0f, 0.0f, 1.0f)},
    {DataType::Vector2, Color(1.0f, 1.0f, 0.0f)},
    {DataType::Vector3, Color(1.0f, 0.8f, 0.0f)},
    {DataType::Color, Color(0.8f, 0.4f, 0.8f)},
    {DataType::Object, Color(0.0f, 0.8f, 1.0f)},
    {DataType::Array, Color(0.6f, 0.6f, 1.0f)},
    {DataType::Execution, Color(1.0f, 1.0f, 1.0f)}};

std::string DataTypeUtils::getTypeName(DataType type) {
    auto it = s_typeNames.find(type);
    return it != s_typeNames.end() ? it->second : "Unknown";
}

bool DataTypeUtils::areTypesCompatible(DataType from, DataType to) {
    if (from == to) {
        return true;
    }

    // 数值类型间的兼容性
    if (isNumericType(from) && isNumericType(to)) {
        return true;
    }

    // 向量类型的兼容性
    if (isVectorType(from) && isVectorType(to)) {
        return true;
    }

    // 特殊兼容性规则
    if (from == DataType::String) {
        return true;  // 字符串可以转换为任何类型
    }

    return false;
}

bool DataTypeUtils::tryConvert(const BlueprintValue& from,
                               DataType toType,
                               BlueprintValue& result) {
    return TypeConversionManager::instance().convert(from, toType, result);
}

BlueprintValue DataTypeUtils::getDefaultValue(DataType type) {
    switch (type) {
        case DataType::None:
            return BlueprintValue();
        case DataType::Boolean:
            return BlueprintValue(false);
        case DataType::Integer:
            return BlueprintValue(0);
        case DataType::Float:
            return BlueprintValue(0.0f);
        case DataType::String:
            return BlueprintValue(std::string(""));
        case DataType::Vector2:
            return BlueprintValue(Vector2(0.0f, 0.0f));
        case DataType::Vector3:
            return BlueprintValue(Vector3(0.0f, 0.0f, 0.0f));
        case DataType::Color:
            return BlueprintValue(Color(0.0f, 0.0f, 0.0f, 1.0f));
        case DataType::Object:
            return BlueprintValue(ObjectReference());
        case DataType::Array:
            return BlueprintValue(BlueprintArray());
        case DataType::Execution:
            return BlueprintValue(ExecutionToken(true));
        default:
            return BlueprintValue();
    }
}

bool DataTypeUtils::isNumericType(DataType type) {
    return type == DataType::Integer || type == DataType::Float;
}

bool DataTypeUtils::isVectorType(DataType type) {
    return type == DataType::Vector2 || type == DataType::Vector3;
}

Color DataTypeUtils::getTypeColor(DataType type) {
    auto it = s_typeColors.find(type);
    return it != s_typeColors.end() ? it->second : Color(0.5f, 0.5f, 0.5f);
}

// 显式模板实例化
template BlueprintValue::BlueprintValue(const bool&);
template BlueprintValue::BlueprintValue(const int&);
template BlueprintValue::BlueprintValue(const float&);
template BlueprintValue::BlueprintValue(const std::string&);
template BlueprintValue::BlueprintValue(const Vector2&);
template BlueprintValue::BlueprintValue(const Vector3&);
template BlueprintValue::BlueprintValue(const Color&);
template BlueprintValue::BlueprintValue(const ExecutionToken&);

template bool BlueprintValue::is<bool>() const;
template bool BlueprintValue::is<int>() const;
template bool BlueprintValue::is<float>() const;
template bool BlueprintValue::is<std::string>() const;

template bool BlueprintValue::get<bool>() const;
template int BlueprintValue::get<int>() const;
template float BlueprintValue::get<float>() const;
template std::string BlueprintValue::get<std::string>() const;

template void BlueprintValue::set<bool>(const bool&);
template void BlueprintValue::set<int>(const int&);
template void BlueprintValue::set<float>(const float&);
template void BlueprintValue::set<std::string>(const std::string&);

// TypeConversionManager 实现

TypeConversionManager& TypeConversionManager::instance() {
    static TypeConversionManager instance;
    static bool initialized = false;
    if (!initialized) {
        instance.initializeDefaultConverters();
        initialized = true;
    }
    return instance;
}

void TypeConversionManager::registerConverter(std::unique_ptr<TypeConverter> converter) {
    if (converter) {
        auto key = std::make_pair(converter->getFromType(), converter->getToType());
        m_converters[key] = std::move(converter);
        Logger::info("Registered type converter: " + DataTypeUtils::getTypeName(key.first) +
                     " -> " + DataTypeUtils::getTypeName(key.second));
    }
}

bool TypeConversionManager::convert(const BlueprintValue& from,
                                    DataType toType,
                                    BlueprintValue& result) {
    if (from.getType() == toType) {
        result = from;
        return true;
    }

    auto key = std::make_pair(from.getType(), toType);
    auto it = m_converters.find(key);

    if (it != m_converters.end()) {
        return it->second->convert(from, result);
    }

    return false;
}

bool TypeConversionManager::canConvert(DataType from, DataType to) const {
    if (from == to) {
        return true;
    }

    auto key = std::make_pair(from, to);
    return m_converters.find(key) != m_converters.end();
}

// 基础类型转换器实现
class IntToFloatConverter : public TypeConverter {
  public:
    bool convert(const BlueprintValue& from, BlueprintValue& to) override {
        if (from.is<int>()) {
            to.set(static_cast<float>(from.get<int>()));
            return true;
        }
        return false;
    }

    DataType getFromType() const override {
        return DataType::Integer;
    }
    DataType getToType() const override {
        return DataType::Float;
    }
};

class FloatToIntConverter : public TypeConverter {
  public:
    bool convert(const BlueprintValue& from, BlueprintValue& to) override {
        if (from.is<float>()) {
            to.set(static_cast<int>(from.get<float>()));
            return true;
        }
        return false;
    }

    DataType getFromType() const override {
        return DataType::Float;
    }
    DataType getToType() const override {
        return DataType::Integer;
    }
};

class BoolToStringConverter : public TypeConverter {
  public:
    bool convert(const BlueprintValue& from, BlueprintValue& to) override {
        if (from.is<bool>()) {
            to.set(from.get<bool>() ? std::string("true") : std::string("false"));
            return true;
        }
        return false;
    }

    DataType getFromType() const override {
        return DataType::Boolean;
    }
    DataType getToType() const override {
        return DataType::String;
    }
};

class NumberToStringConverter : public TypeConverter {
  private:
    DataType m_fromType;

  public:
    explicit NumberToStringConverter(DataType fromType) : m_fromType(fromType) {}

    bool convert(const BlueprintValue& from, BlueprintValue& to) override {
        if (m_fromType == DataType::Integer && from.is<int>()) {
            to.set(std::to_string(from.get<int>()));
            return true;
        } else if (m_fromType == DataType::Float && from.is<float>()) {
            to.set(std::to_string(from.get<float>()));
            return true;
        }
        return false;
    }

    DataType getFromType() const override {
        return m_fromType;
    }
    DataType getToType() const override {
        return DataType::String;
    }
};

void TypeConversionManager::initializeDefaultConverters() {
    // 数值类型转换
    registerConverter(std::make_unique<IntToFloatConverter>());
    registerConverter(std::make_unique<FloatToIntConverter>());

    // 转换为字符串
    registerConverter(std::make_unique<BoolToStringConverter>());
    registerConverter(std::make_unique<NumberToStringConverter>(DataType::Integer));
    registerConverter(std::make_unique<NumberToStringConverter>(DataType::Float));

    Logger::info("Default type converters initialized");
}

}  // namespace blueprint
}  // namespace core
}  // namespace oneday
