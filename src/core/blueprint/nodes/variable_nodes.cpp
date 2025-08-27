#include "variable_nodes.h"
#include "../execution_context.h"
#include "../../common/logger.h"

using oneday::core::Logger;

namespace oneday {
namespace core {
namespace blueprint {

// GetVariableNode 实现

GetVariableNode::GetVariableNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::GetVariable) {
}

std::unique_ptr<BaseNode> GetVariableNode::clone() const {
    auto cloned = std::make_unique<GetVariableNode>();
    cloned->m_variableName = m_variableName;
    return cloned;
}

void GetVariableNode::setVariableName(const std::string& name) {
    m_variableName = name;
}

NodeExecutionResult GetVariableNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 从输入获取变量名（如果有的话）
    BlueprintValue nameInput = getInputValue("variable_name");
    std::string varName = m_variableName;
    if (nameInput.is<std::string>() && !nameInput.get<std::string>().empty()) {
        varName = nameInput.get<std::string>();
    }
    
    if (varName.empty()) {
        result.success = false;
        result.errorMessage = "Variable name is empty";
        return result;
    }
    
    // 从执行上下文获取变量值
    BlueprintValue value = context.getVariable(varName);
    setOutputValue("value", value);
    
    Logger::debug("Get variable: " + varName + " = " + value.toString());
    return result;
}

void GetVariableNode::initializePorts() {
    addInputPort("variable_name", "Variable Name", DataType::String, false);
    addOutputPort("value", "Value", DataType::None);
}

// SetVariableNode 实现

SetVariableNode::SetVariableNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::SetVariable) {
}

std::unique_ptr<BaseNode> SetVariableNode::clone() const {
    auto cloned = std::make_unique<SetVariableNode>();
    cloned->m_variableName = m_variableName;
    return cloned;
}

void SetVariableNode::setVariableName(const std::string& name) {
    m_variableName = name;
}

NodeExecutionResult SetVariableNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 获取执行流输入
    BlueprintValue execInput = getInputValue("exec_in");
    if (!execInput.is<ExecutionToken>() || !execInput.get<ExecutionToken>().valid) {
        result.success = false;
        result.errorMessage = "Invalid execution input";
        return result;
    }
    
    // 从输入获取变量名（如果有的话）
    BlueprintValue nameInput = getInputValue("variable_name");
    std::string varName = m_variableName;
    if (nameInput.is<std::string>() && !nameInput.get<std::string>().empty()) {
        varName = nameInput.get<std::string>();
    }
    
    if (varName.empty()) {
        result.success = false;
        result.errorMessage = "Variable name is empty";
        return result;
    }
    
    // 获取要设置的值
    BlueprintValue value = getInputValue("value");
    
    // 设置变量到执行上下文
    context.setVariable(varName, value);
    
    // 设置执行流输出
    setOutputValue("exec_out", BlueprintValue(ExecutionToken(true)));
    setOutputValue("value", value);
    
    Logger::debug("Set variable: " + varName + " = " + value.toString());
    return result;
}

void SetVariableNode::initializePorts() {
    addInputPort("exec_in", "Execute", DataType::Execution, true);
    addInputPort("variable_name", "Variable Name", DataType::String, false);
    addInputPort("value", "Value", DataType::None, true);
    
    addOutputPort("exec_out", "Execute", DataType::Execution);
    addOutputPort("value", "Value", DataType::None);
}

// IncrementVariableNode 实现

IncrementVariableNode::IncrementVariableNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Increment Variable");
}

std::unique_ptr<BaseNode> IncrementVariableNode::clone() const {
    auto cloned = std::make_unique<IncrementVariableNode>();
    cloned->m_variableName = m_variableName;
    return cloned;
}

void IncrementVariableNode::setVariableName(const std::string& name) {
    m_variableName = name;
}

NodeExecutionResult IncrementVariableNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 获取变量名
    BlueprintValue nameInput = getInputValue("variable_name");
    std::string varName = m_variableName;
    if (nameInput.is<std::string>() && !nameInput.get<std::string>().empty()) {
        varName = nameInput.get<std::string>();
    }
    
    if (varName.empty()) {
        result.success = false;
        result.errorMessage = "Variable name is empty";
        return result;
    }
    
    // 获取当前变量值
    BlueprintValue currentValue = context.getVariable(varName);
    BlueprintValue incrementValue = getInputValue("increment");
    
    // 执行增量操作
    BlueprintValue newValue;
    if (currentValue.is<int>() && incrementValue.is<int>()) {
        newValue = BlueprintValue(currentValue.get<int>() + incrementValue.get<int>());
    } else if (currentValue.is<float>() || incrementValue.is<float>()) {
        float current = currentValue.is<int>() ? static_cast<float>(currentValue.get<int>()) : currentValue.get<float>();
        float increment = incrementValue.is<int>() ? static_cast<float>(incrementValue.get<int>()) : incrementValue.get<float>();
        newValue = BlueprintValue(current + increment);
    } else {
        result.success = false;
        result.errorMessage = "Cannot increment non-numeric variable";
        return result;
    }
    
    // 设置新值
    context.setVariable(varName, newValue);
    
    // 设置输出
    setOutputValue("exec_out", BlueprintValue(ExecutionToken(true)));
    setOutputValue("new_value", newValue);
    
    Logger::debug("Increment variable: " + varName + " from " + currentValue.toString() + " to " + newValue.toString());
    return result;
}

void IncrementVariableNode::initializePorts() {
    addInputPort("exec_in", "Execute", DataType::Execution, true);
    addInputPort("variable_name", "Variable Name", DataType::String, false);
    addInputPort("increment", "Increment", DataType::Float, true);
    
    addOutputPort("exec_out", "Execute", DataType::Execution);
    addOutputPort("new_value", "New Value", DataType::None);
}

// VariableExistsNode 实现

VariableExistsNode::VariableExistsNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Variable Exists");
}

std::unique_ptr<BaseNode> VariableExistsNode::clone() const {
    auto cloned = std::make_unique<VariableExistsNode>();
    cloned->m_variableName = m_variableName;
    return cloned;
}

void VariableExistsNode::setVariableName(const std::string& name) {
    m_variableName = name;
}

NodeExecutionResult VariableExistsNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 获取变量名
    BlueprintValue nameInput = getInputValue("variable_name");
    std::string varName = m_variableName;
    if (nameInput.is<std::string>() && !nameInput.get<std::string>().empty()) {
        varName = nameInput.get<std::string>();
    }
    
    if (varName.empty()) {
        result.success = false;
        result.errorMessage = "Variable name is empty";
        return result;
    }
    
    // 检查变量是否存在
    bool exists = context.hasVariable(varName);
    setOutputValue("exists", BlueprintValue(exists));
    
    Logger::debug("Variable exists check: " + varName + " = " + std::to_string(exists));
    return result;
}

void VariableExistsNode::initializePorts() {
    addInputPort("variable_name", "Variable Name", DataType::String, false);
    addOutputPort("exists", "Exists", DataType::Boolean);
}

// DeleteVariableNode 实现

DeleteVariableNode::DeleteVariableNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Delete Variable");
}

std::unique_ptr<BaseNode> DeleteVariableNode::clone() const {
    auto cloned = std::make_unique<DeleteVariableNode>();
    cloned->m_variableName = m_variableName;
    return cloned;
}

void DeleteVariableNode::setVariableName(const std::string& name) {
    m_variableName = name;
}

NodeExecutionResult DeleteVariableNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 获取变量名
    BlueprintValue nameInput = getInputValue("variable_name");
    std::string varName = m_variableName;
    if (nameInput.is<std::string>() && !nameInput.get<std::string>().empty()) {
        varName = nameInput.get<std::string>();
    }
    
    if (varName.empty()) {
        result.success = false;
        result.errorMessage = "Variable name is empty";
        return result;
    }
    
    // 删除变量
    bool deleted = context.deleteVariable(varName);
    
    // 设置输出
    setOutputValue("exec_out", BlueprintValue(ExecutionToken(true)));
    setOutputValue("deleted", BlueprintValue(deleted));
    
    Logger::debug("Delete variable: " + varName + " = " + std::to_string(deleted));
    return result;
}

void DeleteVariableNode::initializePorts() {
    addInputPort("exec_in", "Execute", DataType::Execution, true);
    addInputPort("variable_name", "Variable Name", DataType::String, false);
    
    addOutputPort("exec_out", "Execute", DataType::Execution);
    addOutputPort("deleted", "Deleted", DataType::Boolean);
}

// GetAllVariablesNode 实现

GetAllVariablesNode::GetAllVariablesNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Get All Variables");
}

std::unique_ptr<BaseNode> GetAllVariablesNode::clone() const {
    return std::make_unique<GetAllVariablesNode>();
}

NodeExecutionResult GetAllVariablesNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 获取所有变量名
    std::vector<std::string> variableNames = context.getAllVariableNames();
    
    // 转换为BlueprintArray
    BlueprintArray nameArray;
    for (const auto& name : variableNames) {
        nameArray.push_back(BlueprintValue(name));
    }
    
    setOutputValue("variable_names", BlueprintValue(nameArray));
    setOutputValue("count", BlueprintValue(static_cast<int>(variableNames.size())));
    
    Logger::debug("Get all variables: found " + std::to_string(variableNames.size()) + " variables");
    return result;
}

void GetAllVariablesNode::initializePorts() {
    addOutputPort("variable_names", "Variable Names", DataType::Array);
    addOutputPort("count", "Count", DataType::Integer);
}

} // namespace blueprint
} // namespace core
} // namespace oneday
