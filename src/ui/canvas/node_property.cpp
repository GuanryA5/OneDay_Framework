#include "node_property.h"
#include <QColor>
#include <iostream>

namespace oneday::ui::canvas {

// NodeProperties 实现
void NodeProperties::addPropertyDefinition(const PropertyDefinition& definition) {
    m_definitions[definition.name] = definition;
    
    // 如果还没有值，设置默认值
    if (!m_values.contains(definition.name)) {
        m_values[definition.name] = PropertyValue(definition.name, definition.defaultValue);
    }
}

PropertyDefinition NodeProperties::getPropertyDefinition(const QString& name) const {
    return m_definitions.value(name);
}

QList<PropertyDefinition> NodeProperties::getAllPropertyDefinitions() const {
    return m_definitions.values();
}

QList<PropertyDefinition> NodeProperties::getPropertyDefinitionsByCategory(const QString& category) const {
    QList<PropertyDefinition> result;
    for (const auto& def : m_definitions.values()) {
        if (def.category == category) {
            result.append(def);
        }
    }
    return result;
}

void NodeProperties::setPropertyValue(const QString& name, const QVariant& value) {
    if (m_definitions.contains(name)) {
        m_values[name].setValue(value);
        std::cout << "Property set - Name: " << name.toStdString() 
                  << ", Value: " << value.toString().toStdString() << std::endl;
    }
}

QVariant NodeProperties::getPropertyValue(const QString& name) const {
    return m_values.value(name).value;
}

PropertyValue NodeProperties::getProperty(const QString& name) const {
    return m_values.value(name);
}

QHash<QString, PropertyValue> NodeProperties::getAllProperties() const {
    return m_values;
}

bool NodeProperties::hasProperty(const QString& name) const {
    return m_definitions.contains(name);
}

bool NodeProperties::isPropertyModified(const QString& name) const {
    return m_values.value(name).modified;
}

void NodeProperties::resetModifiedFlags() {
    for (auto& value : m_values) {
        value.modified = false;
    }
}

void NodeProperties::resetToDefaults() {
    for (auto it = m_values.begin(); it != m_values.end(); ++it) {
        const QString& name = it.key();
        if (m_definitions.contains(name)) {
            it->value = m_definitions[name].defaultValue;
            it->modified = false;
        }
    }
}

QStringList NodeProperties::getCategories() const {
    QStringList categories;
    for (const auto& def : m_definitions.values()) {
        if (!categories.contains(def.category)) {
            categories.append(def.category);
        }
    }
    categories.sort();
    return categories;
}

// NodePropertyFactory 实现
NodeProperties NodePropertyFactory::createPropertiesForNodeType(const QString& nodeType) {
    std::cout << "Creating properties for node type: " << nodeType.toStdString() << std::endl;
    
    NodeProperties properties;
    
    // 添加通用属性
    addCommonProperties(properties);
    
    // 根据节点类型添加特定属性
    if (nodeType.startsWith("event_")) {
        properties = createEventNodeProperties(nodeType);
    } else if (nodeType.startsWith("action_")) {
        properties = createActionNodeProperties(nodeType);
    } else if (nodeType.startsWith("control_")) {
        properties = createControlNodeProperties(nodeType);
    } else if (nodeType.startsWith("data_")) {
        properties = createDataNodeProperties(nodeType);
    } else if (nodeType.startsWith("gameobject_")) {
        properties = createGameObjectNodeProperties(nodeType);
    } else if (nodeType.startsWith("ai_")) {
        properties = createAINodeProperties(nodeType);
    } else if (nodeType.startsWith("image_")) {
        properties = createImageNodeProperties(nodeType);
    }
    
    // 添加外观属性
    addAppearanceProperties(properties);
    
    return properties;
}

NodeProperties NodePropertyFactory::createEventNodeProperties(const QString& nodeType) {
    NodeProperties properties;
    addCommonProperties(properties);
    
    if (nodeType == "event_start") {
        properties.addPropertyDefinition(PropertyDefinition("auto_start", "自动开始", PropertyType::Boolean, true, "Event"));
    } else if (nodeType == "event_keyboard") {
        properties.addPropertyDefinition(PropertyDefinition("key", "按键", PropertyType::Enum, "Space", "Event"));
        auto& keyDef = properties.m_definitions["key"];
        keyDef.enumOptions = {"Space", "Enter", "Escape", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
        
        properties.addPropertyDefinition(PropertyDefinition("key_state", "按键状态", PropertyType::Enum, "Pressed", "Event"));
        auto& stateDef = properties.m_definitions["key_state"];
        stateDef.enumOptions = {"Pressed", "Released", "Held"};
    } else if (nodeType == "event_mouse") {
        properties.addPropertyDefinition(PropertyDefinition("button", "鼠标按键", PropertyType::Enum, "Left", "Event"));
        auto& buttonDef = properties.m_definitions["button"];
        buttonDef.enumOptions = {"Left", "Right", "Middle"};
        
        properties.addPropertyDefinition(PropertyDefinition("button_state", "按键状态", PropertyType::Enum, "Clicked", "Event"));
        auto& stateDef = properties.m_definitions["button_state"];
        stateDef.enumOptions = {"Clicked", "Pressed", "Released", "DoubleClicked"};
    } else if (nodeType == "event_collision") {
        properties.addPropertyDefinition(PropertyDefinition("collision_type", "碰撞类型", PropertyType::Enum, "Enter", "Event"));
        auto& typeDef = properties.m_definitions["collision_type"];
        typeDef.enumOptions = {"Enter", "Exit", "Stay"};
        
        properties.addPropertyDefinition(PropertyDefinition("target_tag", "目标标签", PropertyType::String, "", "Event"));
    }
    
    return properties;
}

NodeProperties NodePropertyFactory::createActionNodeProperties(const QString& nodeType) {
    NodeProperties properties;
    addCommonProperties(properties);
    
    if (nodeType == "action_move") {
        properties.addPropertyDefinition(PropertyDefinition("target_x", "目标X坐标", PropertyType::Double, 0.0, "Movement"));
        properties.addPropertyDefinition(PropertyDefinition("target_y", "目标Y坐标", PropertyType::Double, 0.0, "Movement"));
        properties.addPropertyDefinition(PropertyDefinition("speed", "移动速度", PropertyType::Double, 100.0, "Movement"));
        properties.addPropertyDefinition(PropertyDefinition("relative", "相对移动", PropertyType::Boolean, false, "Movement"));
        
        // 设置数值范围
        properties.m_definitions["target_x"].minValue = -9999.0;
        properties.m_definitions["target_x"].maxValue = 9999.0;
        properties.m_definitions["target_y"].minValue = -9999.0;
        properties.m_definitions["target_y"].maxValue = 9999.0;
        properties.m_definitions["speed"].minValue = 0.1;
        properties.m_definitions["speed"].maxValue = 1000.0;
    } else if (nodeType == "action_rotate") {
        properties.addPropertyDefinition(PropertyDefinition("angle", "旋转角度", PropertyType::Double, 0.0, "Rotation"));
        properties.addPropertyDefinition(PropertyDefinition("speed", "旋转速度", PropertyType::Double, 90.0, "Rotation"));
        properties.addPropertyDefinition(PropertyDefinition("relative", "相对旋转", PropertyType::Boolean, true, "Rotation"));
        
        properties.m_definitions["angle"].minValue = -360.0;
        properties.m_definitions["angle"].maxValue = 360.0;
        properties.m_definitions["speed"].minValue = 0.1;
        properties.m_definitions["speed"].maxValue = 360.0;
    } else if (nodeType == "action_scale") {
        properties.addPropertyDefinition(PropertyDefinition("scale_x", "X轴缩放", PropertyType::Double, 1.0, "Scale"));
        properties.addPropertyDefinition(PropertyDefinition("scale_y", "Y轴缩放", PropertyType::Double, 1.0, "Scale"));
        properties.addPropertyDefinition(PropertyDefinition("uniform", "等比缩放", PropertyType::Boolean, true, "Scale"));
        
        properties.m_definitions["scale_x"].minValue = 0.1;
        properties.m_definitions["scale_x"].maxValue = 10.0;
        properties.m_definitions["scale_y"].minValue = 0.1;
        properties.m_definitions["scale_y"].maxValue = 10.0;
    } else if (nodeType == "action_play_sound") {
        properties.addPropertyDefinition(PropertyDefinition("sound_file", "音频文件", PropertyType::String, "", "Audio"));
        properties.addPropertyDefinition(PropertyDefinition("volume", "音量", PropertyType::Double, 1.0, "Audio"));
        properties.addPropertyDefinition(PropertyDefinition("loop", "循环播放", PropertyType::Boolean, false, "Audio"));
        
        properties.m_definitions["volume"].minValue = 0.0;
        properties.m_definitions["volume"].maxValue = 1.0;
    }
    
    return properties;
}

NodeProperties NodePropertyFactory::createControlNodeProperties(const QString& nodeType) {
    NodeProperties properties;
    addCommonProperties(properties);
    
    if (nodeType == "control_if") {
        properties.addPropertyDefinition(PropertyDefinition("condition", "条件表达式", PropertyType::String, "", "Logic"));
    } else if (nodeType == "control_loop") {
        properties.addPropertyDefinition(PropertyDefinition("loop_type", "循环类型", PropertyType::Enum, "Count", "Loop"));
        auto& typeDef = properties.m_definitions["loop_type"];
        typeDef.enumOptions = {"Count", "While", "Forever"};
        
        properties.addPropertyDefinition(PropertyDefinition("count", "循环次数", PropertyType::Integer, 1, "Loop"));
        properties.addPropertyDefinition(PropertyDefinition("condition", "循环条件", PropertyType::String, "", "Loop"));
        
        properties.m_definitions["count"].minValue = 1;
        properties.m_definitions["count"].maxValue = 10000;
    } else if (nodeType == "control_delay") {
        properties.addPropertyDefinition(PropertyDefinition("delay_time", "延迟时间(秒)", PropertyType::Double, 1.0, "Timing"));
        properties.m_definitions["delay_time"].minValue = 0.0;
        properties.m_definitions["delay_time"].maxValue = 3600.0;
    }
    
    return properties;
}

NodeProperties NodePropertyFactory::createDataNodeProperties(const QString& nodeType) {
    NodeProperties properties;
    addCommonProperties(properties);
    
    if (nodeType == "data_variable") {
        properties.addPropertyDefinition(PropertyDefinition("variable_name", "变量名", PropertyType::String, "myVariable", "Data"));
        properties.addPropertyDefinition(PropertyDefinition("data_type", "数据类型", PropertyType::Enum, "String", "Data"));
        auto& typeDef = properties.m_definitions["data_type"];
        typeDef.enumOptions = {"String", "Integer", "Double", "Boolean"};
        
        properties.addPropertyDefinition(PropertyDefinition("initial_value", "初始值", PropertyType::String, "", "Data"));
    } else if (nodeType == "data_constant") {
        properties.addPropertyDefinition(PropertyDefinition("data_type", "数据类型", PropertyType::Enum, "Integer", "Data"));
        auto& typeDef = properties.m_definitions["data_type"];
        typeDef.enumOptions = {"String", "Integer", "Double", "Boolean"};
        
        properties.addPropertyDefinition(PropertyDefinition("value", "常量值", PropertyType::String, "0", "Data"));
    }
    
    return properties;
}

NodeProperties NodePropertyFactory::createGameObjectNodeProperties(const QString& nodeType) {
    NodeProperties properties;
    addCommonProperties(properties);
    
    properties.addPropertyDefinition(PropertyDefinition("target_object", "目标对象", PropertyType::String, "", "Target"));
    
    return properties;
}

NodeProperties NodePropertyFactory::createAINodeProperties(const QString& nodeType) {
    NodeProperties properties;
    addCommonProperties(properties);
    
    if (nodeType == "ai_pathfinding") {
        properties.addPropertyDefinition(PropertyDefinition("algorithm", "寻路算法", PropertyType::Enum, "A*", "AI"));
        auto& algDef = properties.m_definitions["algorithm"];
        algDef.enumOptions = {"A*", "Dijkstra", "BFS"};
    }
    
    return properties;
}

NodeProperties NodePropertyFactory::createImageNodeProperties(const QString& nodeType) {
    NodeProperties properties;
    addCommonProperties(properties);
    
    if (nodeType == "image_screenshot") {
        properties.addPropertyDefinition(PropertyDefinition("save_path", "保存路径", PropertyType::String, "", "Image"));
        properties.addPropertyDefinition(PropertyDefinition("format", "图像格式", PropertyType::Enum, "PNG", "Image"));
        auto& formatDef = properties.m_definitions["format"];
        formatDef.enumOptions = {"PNG", "JPEG", "BMP"};
    }
    
    return properties;
}

void NodePropertyFactory::addCommonProperties(NodeProperties& properties) {
    properties.addPropertyDefinition(PropertyDefinition("name", "节点名称", PropertyType::String, "Node", "Basic"));
    properties.addPropertyDefinition(PropertyDefinition("enabled", "启用", PropertyType::Boolean, true, "Basic"));
    properties.addPropertyDefinition(PropertyDefinition("description", "描述", PropertyType::String, "", "Basic"));
}

void NodePropertyFactory::addAppearanceProperties(NodeProperties& properties) {
    properties.addPropertyDefinition(PropertyDefinition("node_color", "节点颜色", PropertyType::Color, QColor(74, 144, 226), "Appearance"));
    properties.addPropertyDefinition(PropertyDefinition("show_label", "显示标签", PropertyType::Boolean, true, "Appearance"));
    properties.addPropertyDefinition(PropertyDefinition("label_size", "标签大小", PropertyType::Integer, 10, "Appearance"));
    
    properties.m_definitions["label_size"].minValue = 8;
    properties.m_definitions["label_size"].maxValue = 24;
}

}  // namespace oneday::ui::canvas
