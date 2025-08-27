#include "base_node.h"
#include "../../common/logger.h"
#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>

using oneday::core::Logger;

namespace oneday {
namespace core {
namespace blueprint {

// BaseNode 实现

BaseNode::BaseNode(const std::string& id, NodeType type) 
    : m_id(id), m_type(type), m_state(NodeState::Idle) {
    m_name = NodeUtils::getNodeTypeName(type);
    initializePorts();
    Logger::debug("Created node: " + m_id + " (" + m_name + ")");
}

const NodePort* BaseNode::findInputPort(const std::string& portId) const {
    auto it = std::find_if(m_inputPorts.begin(), m_inputPorts.end(),
        [&portId](const NodePort& port) { return port.id == portId; });
    return it != m_inputPorts.end() ? &(*it) : nullptr;
}

const NodePort* BaseNode::findOutputPort(const std::string& portId) const {
    auto it = std::find_if(m_outputPorts.begin(), m_outputPorts.end(),
        [&portId](const NodePort& port) { return port.id == portId; });
    return it != m_outputPorts.end() ? &(*it) : nullptr;
}

void BaseNode::setInputValue(const std::string& portId, const BlueprintValue& value) {
    const NodePort* port = findInputPort(portId);
    if (port) {
        // 类型检查
        if (port->dataType != DataType::None && value.getType() != port->dataType) {
            // 尝试类型转换
            BlueprintValue convertedValue;
            if (DataTypeUtils::tryConvert(value, port->dataType, convertedValue)) {
                m_inputValues[portId] = convertedValue;
            } else {
                Logger::warning("Type mismatch for input port " + portId + " in node " + m_id);
                return;
            }
        } else {
            m_inputValues[portId] = value;
        }
        Logger::debug("Set input value for port " + portId + " in node " + m_id);
    } else {
        Logger::error("Input port " + portId + " not found in node " + m_id);
    }
}

BlueprintValue BaseNode::getInputValue(const std::string& portId) const {
    auto it = m_inputValues.find(portId);
    if (it != m_inputValues.end()) {
        return it->second;
    }
    
    // 返回端口的默认值
    const NodePort* port = findInputPort(portId);
    if (port && !port->defaultValue.isEmpty()) {
        return port->defaultValue;
    }
    
    // 返回类型的默认值
    if (port) {
        return DataTypeUtils::getDefaultValue(port->dataType);
    }
    
    return BlueprintValue();
}

BlueprintValue BaseNode::getOutputValue(const std::string& portId) const {
    auto it = m_outputValues.find(portId);
    if (it != m_outputValues.end()) {
        return it->second;
    }
    
    const NodePort* port = findOutputPort(portId);
    if (port) {
        return DataTypeUtils::getDefaultValue(port->dataType);
    }
    
    return BlueprintValue();
}

bool BaseNode::canExecute() const {
    if (m_state == NodeState::Disabled || m_state == NodeState::Executing) {
        return false;
    }
    
    // 检查必需的输入端口是否都有值
    for (const auto& port : m_inputPorts) {
        if (port.isRequired) {
            auto it = m_inputValues.find(port.id);
            if (it == m_inputValues.end() || it->second.isEmpty()) {
                return false;
            }
        }
    }
    
    return true;
}

NodeExecutionResult BaseNode::execute(ExecutionContext& context) {
    NodeExecutionResult result;
    
    if (!canExecute()) {
        result.success = false;
        result.errorMessage = "Node cannot execute: " + m_id;
        return result;
    }
    
    setState(NodeState::Executing);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        result = executeInternal(context);
        
        if (result.success) {
            setState(NodeState::Completed);
            // 复制输出值
            result.outputValues = m_outputValues;
        } else {
            setState(NodeState::Error);
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = "Exception in node " + m_id + ": " + e.what();
        setState(NodeState::Error);
        Logger::error(result.errorMessage);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    Logger::debug("Node " + m_id + " executed in " + std::to_string(result.executionTime) + "ms");
    
    return result;
}

void BaseNode::reset() {
    setState(NodeState::Idle);
    m_outputValues.clear();
    Logger::debug("Reset node: " + m_id);
}

bool BaseNode::validate(std::string& errorMessage) const {
    // 检查必需的输入端口
    for (const auto& port : m_inputPorts) {
        if (port.isRequired) {
            auto it = m_inputValues.find(port.id);
            if (it == m_inputValues.end() || it->second.isEmpty()) {
                errorMessage = "Required input port '" + port.name + "' is not connected";
                return false;
            }
        }
    }
    
    return true;
}

std::map<std::string, BlueprintValue> BaseNode::serialize() const {
    std::map<std::string, BlueprintValue> data;
    
    data["id"] = BlueprintValue(m_id);
    data["type"] = BlueprintValue(static_cast<int>(m_type));
    data["name"] = BlueprintValue(m_name);
    data["description"] = BlueprintValue(m_description);
    
    // 序列化输入值
    for (const auto& pair : m_inputValues) {
        data["input_" + pair.first] = pair.second;
    }
    
    return data;
}

bool BaseNode::deserialize(const std::map<std::string, BlueprintValue>& data) {
    try {
        auto it = data.find("name");
        if (it != data.end() && it->second.is<std::string>()) {
            m_name = it->second.get<std::string>();
        }
        
        it = data.find("description");
        if (it != data.end() && it->second.is<std::string>()) {
            m_description = it->second.get<std::string>();
        }
        
        // 反序列化输入值
        for (const auto& pair : data) {
            if (pair.first.substr(0, 6) == "input_") {
                std::string portId = pair.first.substr(6);
                m_inputValues[portId] = pair.second;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        Logger::error("Failed to deserialize node " + m_id + ": " + e.what());
        return false;
    }
}

void BaseNode::addInputPort(const std::string& id, const std::string& name, DataType type, bool required) {
    NodePort port(id, name, type, PortType::Input);
    port.isRequired = required;
    port.defaultValue = DataTypeUtils::getDefaultValue(type);
    m_inputPorts.push_back(port);
    Logger::debug("Added input port: " + id + " to node " + m_id);
}

void BaseNode::addOutputPort(const std::string& id, const std::string& name, DataType type) {
    NodePort port(id, name, type, PortType::Output);
    m_outputPorts.push_back(port);
    Logger::debug("Added output port: " + id + " to node " + m_id);
}

void BaseNode::setOutputValue(const std::string& portId, const BlueprintValue& value) {
    const NodePort* port = findOutputPort(portId);
    if (port) {
        m_outputValues[portId] = value;
        Logger::debug("Set output value for port " + portId + " in node " + m_id);
    } else {
        Logger::error("Output port " + portId + " not found in node " + m_id);
    }
}

// NodeUtils 实现

const std::map<NodeType, std::string> NodeUtils::s_nodeTypeNames = {
    {NodeType::Start, "Start"},
    {NodeType::End, "End"},
    {NodeType::Branch, "Branch"},
    {NodeType::Loop, "Loop"},
    {NodeType::Delay, "Delay"},
    {NodeType::And, "And"},
    {NodeType::Or, "Or"},
    {NodeType::Not, "Not"},
    {NodeType::Compare, "Compare"},
    {NodeType::Add, "Add"},
    {NodeType::Subtract, "Subtract"},
    {NodeType::Multiply, "Multiply"},
    {NodeType::Divide, "Divide"},
    {NodeType::GetVariable, "Get Variable"},
    {NodeType::SetVariable, "Set Variable"},
    {NodeType::Custom, "Custom"}
};

const std::map<NodeType, Color> NodeUtils::s_nodeTypeColors = {
    {NodeType::Start, Color(0.0f, 1.0f, 0.0f)},        // 绿色
    {NodeType::End, Color(1.0f, 0.0f, 0.0f)},          // 红色
    {NodeType::Branch, Color(1.0f, 1.0f, 0.0f)},       // 黄色
    {NodeType::Loop, Color(1.0f, 0.5f, 0.0f)},         // 橙色
    {NodeType::Delay, Color(0.5f, 0.5f, 1.0f)},        // 浅蓝色
    {NodeType::And, Color(0.8f, 0.2f, 0.8f)},          // 紫色
    {NodeType::Or, Color(0.8f, 0.2f, 0.8f)},           // 紫色
    {NodeType::Not, Color(0.8f, 0.2f, 0.8f)},          // 紫色
    {NodeType::Compare, Color(0.8f, 0.2f, 0.8f)},      // 紫色
    {NodeType::Add, Color(0.2f, 0.8f, 0.2f)},          // 浅绿色
    {NodeType::Subtract, Color(0.2f, 0.8f, 0.2f)},     // 浅绿色
    {NodeType::Multiply, Color(0.2f, 0.8f, 0.2f)},     // 浅绿色
    {NodeType::Divide, Color(0.2f, 0.8f, 0.2f)},       // 浅绿色
    {NodeType::GetVariable, Color(0.2f, 0.8f, 0.8f)},  // 青色
    {NodeType::SetVariable, Color(0.2f, 0.8f, 0.8f)},  // 青色
    {NodeType::Custom, Color(0.5f, 0.5f, 0.5f)}        // 灰色
};

std::string NodeUtils::getNodeTypeName(NodeType type) {
    auto it = s_nodeTypeNames.find(type);
    return it != s_nodeTypeNames.end() ? it->second : "Unknown";
}

Color NodeUtils::getNodeTypeColor(NodeType type) {
    auto it = s_nodeTypeColors.find(type);
    return it != s_nodeTypeColors.end() ? it->second : Color(0.5f, 0.5f, 0.5f);
}

std::string NodeUtils::generateNodeId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::ostringstream oss;
    oss << "node_";
    for (int i = 0; i < 8; ++i) {
        oss << std::hex << dis(gen);
    }
    return oss.str();
}

std::string NodeUtils::generateConnectionId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::ostringstream oss;
    oss << "conn_";
    for (int i = 0; i < 8; ++i) {
        oss << std::hex << dis(gen);
    }
    return oss.str();
}

bool NodeUtils::isValidConnection(const NodePort& sourcePort, const NodePort& targetPort) {
    // 检查端口类型
    if (sourcePort.portType != PortType::Output || targetPort.portType != PortType::Input) {
        return false;
    }
    
    // 检查数据类型兼容性
    return DataTypeUtils::areTypesCompatible(sourcePort.dataType, targetPort.dataType);
}

} // namespace blueprint
} // namespace core
} // namespace oneday
