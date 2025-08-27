#pragma once

#include <QHash>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QVariant>

namespace oneday::ui::canvas {

/**
 * @brief 属性类型枚举
 */
enum class PropertyType {
    String,   ///< 字符串类型
    Integer,  ///< 整数类型
    Double,   ///< 浮点数类型
    Boolean,  ///< 布尔类型
    Enum,     ///< 枚举类型
    Color,    ///< 颜色类型
    Vector2,  ///< 二维向量类型
    Vector3   ///< 三维向量类型
};

/**
 * @brief 属性定义结构
 *
 * 定义一个属性的元数据信息
 */
struct PropertyDefinition {
    QString name;             ///< 属性名称
    QString displayName;      ///< 显示名称
    QString description;      ///< 属性描述
    PropertyType type;        ///< 属性类型
    QVariant defaultValue;    ///< 默认值
    QVariant minValue;        ///< 最小值（数值类型）
    QVariant maxValue;        ///< 最大值（数值类型）
    QStringList enumOptions;  ///< 枚举选项（枚举类型）
    QString category;         ///< 属性分类
    bool readOnly;            ///< 是否只读
    bool visible;             ///< 是否可见

    PropertyDefinition() : type(PropertyType::String), readOnly(false), visible(true) {}

    PropertyDefinition(const QString& name,
                       const QString& displayName,
                       PropertyType type,
                       const QVariant& defaultValue = QVariant(),
                       const QString& category = "Basic")
        : name(name),
          displayName(displayName),
          type(type),
          defaultValue(defaultValue),
          category(category),
          readOnly(false),
          visible(true) {}
};

/**
 * @brief 属性值结构
 *
 * 存储一个属性的实际值
 */
struct PropertyValue {
    QString name;    ///< 属性名称
    QVariant value;  ///< 属性值
    bool modified;   ///< 是否已修改

    PropertyValue() : modified(false) {}

    PropertyValue(const QString& name, const QVariant& value)
        : name(name), value(value), modified(false) {}

    void setValue(const QVariant& newValue) {
        if (value != newValue) {
            value = newValue;
            modified = true;
        }
    }
};

/**
 * @brief 节点属性集合
 *
 * 管理一个节点的所有属性
 */
class NodeProperties {
  public:
    NodeProperties() = default;
    ~NodeProperties() = default;

    /**
     * @brief 添加属性定义
     */
    void addPropertyDefinition(const PropertyDefinition& definition);

    /**
     * @brief 获取属性定义
     */
    PropertyDefinition getPropertyDefinition(const QString& name) const;

    /**
     * @brief 获取所有属性定义
     */
    QList<PropertyDefinition> getAllPropertyDefinitions() const;

    /**
     * @brief 按分类获取属性定义
     */
    QList<PropertyDefinition> getPropertyDefinitionsByCategory(const QString& category) const;

    /**
     * @brief 设置属性值
     */
    void setPropertyValue(const QString& name, const QVariant& value);

    /**
     * @brief 获取属性值
     */
    QVariant getPropertyValue(const QString& name) const;

    /**
     * @brief 获取属性值对象
     */
    PropertyValue getProperty(const QString& name) const;

    /**
     * @brief 获取所有属性值
     */
    QHash<QString, PropertyValue> getAllProperties() const;

    /**
     * @brief 检查属性是否存在
     */
    bool hasProperty(const QString& name) const;

    /**
     * @brief 检查属性是否已修改
     */
    bool isPropertyModified(const QString& name) const;

    /**
     * @brief 重置属性修改标记
     */
    void resetModifiedFlags();

    /**
     * @brief 重置属性到默认值
     */
    void resetToDefaults();

    /**
     * @brief 获取所有分类
     */
    QStringList getCategories() const;

  private:
    friend class NodePropertyFactory;  ///< 允许工厂类访问私有成员

    QHash<QString, PropertyDefinition> m_definitions;  ///< 属性定义
    QHash<QString, PropertyValue> m_values;            ///< 属性值
};

/**
 * @brief 节点属性工厂
 *
 * 根据节点类型创建对应的属性集合
 */
class NodePropertyFactory {
  public:
    /**
     * @brief 为指定节点类型创建属性集合
     */
    static NodeProperties createPropertiesForNodeType(const QString& nodeType);

  private:
    /**
     * @brief 创建事件节点属性
     */
    static NodeProperties createEventNodeProperties(const QString& nodeType);

    /**
     * @brief 创建动作节点属性
     */
    static NodeProperties createActionNodeProperties(const QString& nodeType);

    /**
     * @brief 创建控制流节点属性
     */
    static NodeProperties createControlNodeProperties(const QString& nodeType);

    /**
     * @brief 创建数据节点属性
     */
    static NodeProperties createDataNodeProperties(const QString& nodeType);

    /**
     * @brief 创建游戏对象节点属性
     */
    static NodeProperties createGameObjectNodeProperties(const QString& nodeType);

    /**
     * @brief 创建AI节点属性
     */
    static NodeProperties createAINodeProperties(const QString& nodeType);

    /**
     * @brief 创建图像处理节点属性
     */
    static NodeProperties createImageNodeProperties(const QString& nodeType);

    /**
     * @brief 添加通用属性
     */
    static void addCommonProperties(NodeProperties& properties);

    /**
     * @brief 添加外观属性
     */
    static void addAppearanceProperties(NodeProperties& properties);
};

}  // namespace oneday::ui::canvas

// 注册元类型
Q_DECLARE_METATYPE(oneday::ui::canvas::PropertyType)
Q_DECLARE_METATYPE(oneday::ui::canvas::PropertyDefinition)
Q_DECLARE_METATYPE(oneday::ui::canvas::PropertyValue)
