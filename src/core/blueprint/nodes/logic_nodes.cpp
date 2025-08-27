#include "logic_nodes.h"

#include "../../common/logger.h"
#include "../execution_context.h"

using oneday::core::Logger;

namespace oneday {
namespace core {
namespace blueprint {

// AndNode 实现

AndNode::AndNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::And) {}

std::unique_ptr<BaseNode> AndNode::clone() const {
    return std::make_unique<AndNode>();
}

NodeExecutionResult AndNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    bool a = getInputValue("a").get<bool>();
    bool b = getInputValue("b").get<bool>();
    bool output = a && b;

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("And node: " + std::to_string(a) + " && " + std::to_string(b) + " = " +
                  std::to_string(output));
    return result;
}

void AndNode::initializePorts() {
    addInputPort("a", "A", DataType::Boolean, true);
    addInputPort("b", "B", DataType::Boolean, true);
    addOutputPort("result", "Result", DataType::Boolean);
}

// OrNode 实现

OrNode::OrNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Or) {}

std::unique_ptr<BaseNode> OrNode::clone() const {
    return std::make_unique<OrNode>();
}

NodeExecutionResult OrNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    bool a = getInputValue("a").get<bool>();
    bool b = getInputValue("b").get<bool>();
    bool output = a || b;

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Or node: " + std::to_string(a) + " || " + std::to_string(b) + " = " +
                  std::to_string(output));
    return result;
}

void OrNode::initializePorts() {
    addInputPort("a", "A", DataType::Boolean, true);
    addInputPort("b", "B", DataType::Boolean, true);
    addOutputPort("result", "Result", DataType::Boolean);
}

// NotNode 实现

NotNode::NotNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Not) {}

std::unique_ptr<BaseNode> NotNode::clone() const {
    return std::make_unique<NotNode>();
}

NodeExecutionResult NotNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    bool input = getInputValue("input").get<bool>();
    bool output = !input;

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Not node: !" + std::to_string(input) + " = " + std::to_string(output));
    return result;
}

void NotNode::initializePorts() {
    addInputPort("input", "Input", DataType::Boolean, true);
    addOutputPort("result", "Result", DataType::Boolean);
}

// CompareNode 实现

CompareNode::CompareNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Compare) {}

std::unique_ptr<BaseNode> CompareNode::clone() const {
    auto cloned = std::make_unique<CompareNode>();
    cloned->m_operation = m_operation;
    return cloned;
}

void CompareNode::setOperation(CompareOperation op) {
    m_operation = op;
}

NodeExecutionResult CompareNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    BlueprintValue a = getInputValue("a");
    BlueprintValue b = getInputValue("b");

    bool comparisonResult = performComparison(a, b);
    setOutputValue("result", BlueprintValue(comparisonResult));

    Logger::debug("Compare node: comparison result = " + std::to_string(comparisonResult));
    return result;
}

void CompareNode::initializePorts() {
    addInputPort("a", "A", DataType::None, true);  // 接受任何类型
    addInputPort("b", "B", DataType::None, true);  // 接受任何类型
    addOutputPort("result", "Result", DataType::Boolean);
}

bool CompareNode::performComparison(const BlueprintValue& a, const BlueprintValue& b) const {
    // 如果类型不同，尝试转换
    if (a.getType() != b.getType()) {
        // 简化处理：如果都是数值类型，转换为float比较
        if (DataTypeUtils::isNumericType(a.getType()) &&
            DataTypeUtils::isNumericType(b.getType())) {
            float fa = 0.0f, fb = 0.0f;

            if (a.is<int>())
                fa = static_cast<float>(a.get<int>());
            else if (a.is<float>())
                fa = a.get<float>();

            if (b.is<int>())
                fb = static_cast<float>(b.get<int>());
            else if (b.is<float>())
                fb = b.get<float>();

            switch (m_operation) {
                case CompareOperation::Equal:
                    return fa == fb;
                case CompareOperation::NotEqual:
                    return fa != fb;
                case CompareOperation::Greater:
                    return fa > fb;
                case CompareOperation::GreaterOrEqual:
                    return fa >= fb;
                case CompareOperation::Less:
                    return fa < fb;
                case CompareOperation::LessOrEqual:
                    return fa <= fb;
            }
        }
        return false;
    }

    // 相同类型的比较
    switch (m_operation) {
        case CompareOperation::Equal:
            return a == b;
        case CompareOperation::NotEqual:
            return a != b;
        case CompareOperation::Greater:
            if (a.is<int>() && b.is<int>())
                return a.get<int>() > b.get<int>();
            if (a.is<float>() && b.is<float>())
                return a.get<float>() > b.get<float>();
            break;
        case CompareOperation::GreaterOrEqual:
            if (a.is<int>() && b.is<int>())
                return a.get<int>() >= b.get<int>();
            if (a.is<float>() && b.is<float>())
                return a.get<float>() >= b.get<float>();
            break;
        case CompareOperation::Less:
            if (a.is<int>() && b.is<int>())
                return a.get<int>() < b.get<int>();
            if (a.is<float>() && b.is<float>())
                return a.get<float>() < b.get<float>();
            break;
        case CompareOperation::LessOrEqual:
            if (a.is<int>() && b.is<int>())
                return a.get<int>() <= b.get<int>();
            if (a.is<float>() && b.is<float>())
                return a.get<float>() <= b.get<float>();
            break;
    }

    return false;
}

// XorNode 实现

XorNode::XorNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Xor");
}

std::unique_ptr<BaseNode> XorNode::clone() const {
    return std::make_unique<XorNode>();
}

NodeExecutionResult XorNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    bool a = getInputValue("a").get<bool>();
    bool b = getInputValue("b").get<bool>();
    bool output = a ^ b;  // 异或操作

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Xor node: " + std::to_string(a) + " ^ " + std::to_string(b) + " = " +
                  std::to_string(output));
    return result;
}

void XorNode::initializePorts() {
    addInputPort("a", "A", DataType::Boolean, true);
    addInputPort("b", "B", DataType::Boolean, true);
    addOutputPort("result", "Result", DataType::Boolean);
}

// SelectNode 实现

SelectNode::SelectNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Select");
}

std::unique_ptr<BaseNode> SelectNode::clone() const {
    return std::make_unique<SelectNode>();
}

NodeExecutionResult SelectNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    bool condition = getInputValue("condition").get<bool>();
    BlueprintValue trueValue = getInputValue("true_value");
    BlueprintValue falseValue = getInputValue("false_value");

    BlueprintValue selectedValue = condition ? trueValue : falseValue;
    setOutputValue("result", selectedValue);

    Logger::debug("Select node: selected " + std::string(condition ? "true" : "false") + " value");
    return result;
}

void SelectNode::initializePorts() {
    addInputPort("condition", "Condition", DataType::Boolean, true);
    addInputPort("true_value", "True Value", DataType::None, true);
    addInputPort("false_value", "False Value", DataType::None, true);
    addOutputPort("result", "Result", DataType::None);
}

// InRangeNode 实现

InRangeNode::InRangeNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("In Range");
}

std::unique_ptr<BaseNode> InRangeNode::clone() const {
    return std::make_unique<InRangeNode>();
}

NodeExecutionResult InRangeNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float value = 0.0f;
    float minValue = 0.0f;
    float maxValue = 0.0f;

    // 获取并转换值
    BlueprintValue valueInput = getInputValue("value");
    if (valueInput.is<int>())
        value = static_cast<float>(valueInput.get<int>());
    else if (valueInput.is<float>())
        value = valueInput.get<float>();

    BlueprintValue minInput = getInputValue("min");
    if (minInput.is<int>())
        minValue = static_cast<float>(minInput.get<int>());
    else if (minInput.is<float>())
        minValue = minInput.get<float>();

    BlueprintValue maxInput = getInputValue("max");
    if (maxInput.is<int>())
        maxValue = static_cast<float>(maxInput.get<int>());
    else if (maxInput.is<float>())
        maxValue = maxInput.get<float>();

    bool inRange = (value >= minValue) && (value <= maxValue);
    setOutputValue("result", BlueprintValue(inRange));

    Logger::debug("In Range node: " + std::to_string(value) + " in [" + std::to_string(minValue) +
                  ", " + std::to_string(maxValue) + "] = " + std::to_string(inRange));
    return result;
}

void InRangeNode::initializePorts() {
    addInputPort("value", "Value", DataType::Float, true);
    addInputPort("min", "Min", DataType::Float, true);
    addInputPort("max", "Max", DataType::Float, true);
    addOutputPort("result", "In Range", DataType::Boolean);
}

// IsTypeNode 实现

IsTypeNode::IsTypeNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Is Type");
}

std::unique_ptr<BaseNode> IsTypeNode::clone() const {
    auto cloned = std::make_unique<IsTypeNode>();
    cloned->m_targetType = m_targetType;
    return cloned;
}

void IsTypeNode::setTargetType(DataType type) {
    m_targetType = type;
}

NodeExecutionResult IsTypeNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    BlueprintValue input = getInputValue("input");
    bool isTargetType = (input.getType() == m_targetType);

    setOutputValue("result", BlueprintValue(isTargetType));

    Logger::debug("Is Type node: input type is " + DataTypeUtils::getTypeName(input.getType()) +
                  ", target type is " + DataTypeUtils::getTypeName(m_targetType) +
                  ", result = " + std::to_string(isTargetType));
    return result;
}

void IsTypeNode::initializePorts() {
    addInputPort("input", "Input", DataType::None, true);
    addOutputPort("result", "Is Type", DataType::Boolean);
}

}  // namespace blueprint
}  // namespace core
}  // namespace oneday
