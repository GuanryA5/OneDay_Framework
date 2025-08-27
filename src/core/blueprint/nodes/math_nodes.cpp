#include "math_nodes.h"

#include <algorithm>
#include <cmath>
#include <random>

#include "../../common/logger.h"
#include "../execution_context.h"

using oneday::core::Logger;

namespace oneday {
namespace core {
namespace blueprint {

// 辅助函数：获取数值（支持int和float）
float getNumericValue(const BlueprintValue& value) {
    if (value.is<int>()) {
        return static_cast<float>(value.get<int>());
    } else if (value.is<float>()) {
        return value.get<float>();
    }
    return 0.0f;
}

// AddNode 实现

AddNode::AddNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Add) {}

std::unique_ptr<BaseNode> AddNode::clone() const {
    return std::make_unique<AddNode>();
}

NodeExecutionResult AddNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float a = getNumericValue(getInputValue("a"));
    float b = getNumericValue(getInputValue("b"));
    float output = a + b;

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Add node: " + std::to_string(a) + " + " + std::to_string(b) + " = " +
                  std::to_string(output));
    return result;
}

void AddNode::initializePorts() {
    addInputPort("a", "A", DataType::Float, true);
    addInputPort("b", "B", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// SubtractNode 实现

SubtractNode::SubtractNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Subtract) {}

std::unique_ptr<BaseNode> SubtractNode::clone() const {
    return std::make_unique<SubtractNode>();
}

NodeExecutionResult SubtractNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float a = getNumericValue(getInputValue("a"));
    float b = getNumericValue(getInputValue("b"));
    float output = a - b;

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Subtract node: " + std::to_string(a) + " - " + std::to_string(b) + " = " +
                  std::to_string(output));
    return result;
}

void SubtractNode::initializePorts() {
    addInputPort("a", "A", DataType::Float, true);
    addInputPort("b", "B", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// MultiplyNode 实现

MultiplyNode::MultiplyNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Multiply) {}

std::unique_ptr<BaseNode> MultiplyNode::clone() const {
    return std::make_unique<MultiplyNode>();
}

NodeExecutionResult MultiplyNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float a = getNumericValue(getInputValue("a"));
    float b = getNumericValue(getInputValue("b"));
    float output = a * b;

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Multiply node: " + std::to_string(a) + " * " + std::to_string(b) + " = " +
                  std::to_string(output));
    return result;
}

void MultiplyNode::initializePorts() {
    addInputPort("a", "A", DataType::Float, true);
    addInputPort("b", "B", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// DivideNode 实现

DivideNode::DivideNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Divide) {}

std::unique_ptr<BaseNode> DivideNode::clone() const {
    return std::make_unique<DivideNode>();
}

NodeExecutionResult DivideNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float a = getNumericValue(getInputValue("a"));
    float b = getNumericValue(getInputValue("b"));

    if (std::abs(b) < 1e-6f) {
        result.success = false;
        result.errorMessage = "Division by zero";
        Logger::error("Divide node: Division by zero");
        return result;
    }

    float output = a / b;
    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Divide node: " + std::to_string(a) + " / " + std::to_string(b) + " = " +
                  std::to_string(output));
    return result;
}

void DivideNode::initializePorts() {
    addInputPort("a", "A", DataType::Float, true);
    addInputPort("b", "B", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// ModuloNode 实现

ModuloNode::ModuloNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Modulo");
}

std::unique_ptr<BaseNode> ModuloNode::clone() const {
    return std::make_unique<ModuloNode>();
}

NodeExecutionResult ModuloNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    int a = getInputValue("a").get<int>();
    int b = getInputValue("b").get<int>();

    if (b == 0) {
        result.success = false;
        result.errorMessage = "Modulo by zero";
        Logger::error("Modulo node: Modulo by zero");
        return result;
    }

    int output = a % b;
    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Modulo node: " + std::to_string(a) + " % " + std::to_string(b) + " = " +
                  std::to_string(output));
    return result;
}

void ModuloNode::initializePorts() {
    addInputPort("a", "A", DataType::Integer, true);
    addInputPort("b", "B", DataType::Integer, true);
    addOutputPort("result", "Result", DataType::Integer);
}

// PowerNode 实现

PowerNode::PowerNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Power");
}

std::unique_ptr<BaseNode> PowerNode::clone() const {
    return std::make_unique<PowerNode>();
}

NodeExecutionResult PowerNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float base = getNumericValue(getInputValue("base"));
    float exponent = getNumericValue(getInputValue("exponent"));

    float output = std::pow(base, exponent);

    if (std::isnan(output) || std::isinf(output)) {
        result.success = false;
        result.errorMessage = "Invalid power operation result";
        Logger::error("Power node: Invalid result");
        return result;
    }

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Power node: " + std::to_string(base) + " ^ " + std::to_string(exponent) + " = " +
                  std::to_string(output));
    return result;
}

void PowerNode::initializePorts() {
    addInputPort("base", "Base", DataType::Float, true);
    addInputPort("exponent", "Exponent", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// SqrtNode 实现

SqrtNode::SqrtNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Square Root");
}

std::unique_ptr<BaseNode> SqrtNode::clone() const {
    return std::make_unique<SqrtNode>();
}

NodeExecutionResult SqrtNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float input = getNumericValue(getInputValue("input"));

    if (input < 0.0f) {
        result.success = false;
        result.errorMessage = "Square root of negative number";
        Logger::error("Sqrt node: Negative input");
        return result;
    }

    float output = std::sqrt(input);
    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Sqrt node: sqrt(" + std::to_string(input) + ") = " + std::to_string(output));
    return result;
}

void SqrtNode::initializePorts() {
    addInputPort("input", "Input", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// AbsNode 实现

AbsNode::AbsNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Absolute");
}

std::unique_ptr<BaseNode> AbsNode::clone() const {
    return std::make_unique<AbsNode>();
}

NodeExecutionResult AbsNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float input = getNumericValue(getInputValue("input"));
    float output = std::abs(input);

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Abs node: abs(" + std::to_string(input) + ") = " + std::to_string(output));
    return result;
}

void AbsNode::initializePorts() {
    addInputPort("input", "Input", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// MinNode 实现

MinNode::MinNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Minimum");
}

std::unique_ptr<BaseNode> MinNode::clone() const {
    return std::make_unique<MinNode>();
}

NodeExecutionResult MinNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float a = getNumericValue(getInputValue("a"));
    float b = getNumericValue(getInputValue("b"));
    float output = std::min(a, b);

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Min node: min(" + std::to_string(a) + ", " + std::to_string(b) +
                  ") = " + std::to_string(output));
    return result;
}

void MinNode::initializePorts() {
    addInputPort("a", "A", DataType::Float, true);
    addInputPort("b", "B", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// MaxNode 实现

MaxNode::MaxNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Maximum");
}

std::unique_ptr<BaseNode> MaxNode::clone() const {
    return std::make_unique<MaxNode>();
}

NodeExecutionResult MaxNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float a = getNumericValue(getInputValue("a"));
    float b = getNumericValue(getInputValue("b"));
    float output = std::max(a, b);

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Max node: max(" + std::to_string(a) + ", " + std::to_string(b) +
                  ") = " + std::to_string(output));
    return result;
}

void MaxNode::initializePorts() {
    addInputPort("a", "A", DataType::Float, true);
    addInputPort("b", "B", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// ClampNode 实现

ClampNode::ClampNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Clamp");
}

std::unique_ptr<BaseNode> ClampNode::clone() const {
    return std::make_unique<ClampNode>();
}

NodeExecutionResult ClampNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float value = getNumericValue(getInputValue("value"));
    float minVal = getNumericValue(getInputValue("min"));
    float maxVal = getNumericValue(getInputValue("max"));

    float output = std::clamp(value, minVal, maxVal);
    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Clamp node: clamp(" + std::to_string(value) + ", " + std::to_string(minVal) +
                  ", " + std::to_string(maxVal) + ") = " + std::to_string(output));
    return result;
}

void ClampNode::initializePorts() {
    addInputPort("value", "Value", DataType::Float, true);
    addInputPort("min", "Min", DataType::Float, true);
    addInputPort("max", "Max", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// LerpNode 实现

LerpNode::LerpNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Lerp");
}

std::unique_ptr<BaseNode> LerpNode::clone() const {
    return std::make_unique<LerpNode>();
}

NodeExecutionResult LerpNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float a = getNumericValue(getInputValue("a"));
    float b = getNumericValue(getInputValue("b"));
    float t = getNumericValue(getInputValue("t"));

    // 限制t在[0,1]范围内
    t = std::clamp(t, 0.0f, 1.0f);

    float output = a + t * (b - a);
    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Lerp node: lerp(" + std::to_string(a) + ", " + std::to_string(b) + ", " +
                  std::to_string(t) + ") = " + std::to_string(output));
    return result;
}

void LerpNode::initializePorts() {
    addInputPort("a", "A", DataType::Float, true);
    addInputPort("b", "B", DataType::Float, true);
    addInputPort("t", "T", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// TrigNode 实现

TrigNode::TrigNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Trigonometry");
}

std::unique_ptr<BaseNode> TrigNode::clone() const {
    auto cloned = std::make_unique<TrigNode>();
    cloned->m_function = m_function;
    return cloned;
}

void TrigNode::setFunction(TrigFunction func) {
    m_function = func;
}

NodeExecutionResult TrigNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float input = getNumericValue(getInputValue("input"));
    float output = 0.0f;

    switch (m_function) {
        case TrigFunction::Sin:
            output = std::sin(input);
            break;
        case TrigFunction::Cos:
            output = std::cos(input);
            break;
        case TrigFunction::Tan:
            output = std::tan(input);
            break;
        case TrigFunction::Asin:
            if (input < -1.0f || input > 1.0f) {
                result.success = false;
                result.errorMessage = "Asin input out of range [-1, 1]";
                return result;
            }
            output = std::asin(input);
            break;
        case TrigFunction::Acos:
            if (input < -1.0f || input > 1.0f) {
                result.success = false;
                result.errorMessage = "Acos input out of range [-1, 1]";
                return result;
            }
            output = std::acos(input);
            break;
        case TrigFunction::Atan:
            output = std::atan(input);
            break;
    }

    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Trig node: function result = " + std::to_string(output));
    return result;
}

void TrigNode::initializePorts() {
    addInputPort("input", "Input", DataType::Float, true);
    addOutputPort("result", "Result", DataType::Float);
}

// RandomNode 实现

RandomNode::RandomNode(const std::string& id)
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Random");
}

std::unique_ptr<BaseNode> RandomNode::clone() const {
    return std::make_unique<RandomNode>();
}

NodeExecutionResult RandomNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;

    float minVal = getNumericValue(getInputValue("min"));
    float maxVal = getNumericValue(getInputValue("max"));

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(minVal, maxVal);

    float output = dis(gen);
    setOutputValue("result", BlueprintValue(output));

    Logger::debug("Random node: generated " + std::to_string(output) + " in range [" +
                  std::to_string(minVal) + ", " + std::to_string(maxVal) + "]");
    return result;
}

void RandomNode::initializePorts() {
    addInputPort("min", "Min", DataType::Float, false);
    addInputPort("max", "Max", DataType::Float, false);
    addOutputPort("result", "Result", DataType::Float);
}

}  // namespace blueprint
}  // namespace core
}  // namespace oneday
