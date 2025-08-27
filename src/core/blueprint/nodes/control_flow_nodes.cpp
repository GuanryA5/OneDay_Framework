#include "control_flow_nodes.h"
#include "../execution_context.h"
#include "../../common/logger.h"
#include <thread>
#include <chrono>

using oneday::core::Logger;

namespace oneday {
namespace core {
namespace blueprint {

// StartNode 实现

StartNode::StartNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Start) {
}

std::unique_ptr<BaseNode> StartNode::clone() const {
    return std::make_unique<StartNode>();
}

NodeExecutionResult StartNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 设置执行流输出
    setOutputValue("exec_out", BlueprintValue(ExecutionToken(true)));
    
    Logger::info("Start node executed");
    return result;
}

void StartNode::initializePorts() {
    // 开始节点只有执行流输出
    addOutputPort("exec_out", "Execute", DataType::Execution);
}

// EndNode 实现

EndNode::EndNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::End) {
}

std::unique_ptr<BaseNode> EndNode::clone() const {
    return std::make_unique<EndNode>();
}

NodeExecutionResult EndNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    Logger::info("End node executed - blueprint finished");
    return result;
}

void EndNode::initializePorts() {
    // 结束节点只有执行流输入
    addInputPort("exec_in", "Execute", DataType::Execution, true);
}

// BranchNode 实现

BranchNode::BranchNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Branch) {
}

std::unique_ptr<BaseNode> BranchNode::clone() const {
    return std::make_unique<BranchNode>();
}

NodeExecutionResult BranchNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 获取条件值
    BlueprintValue conditionValue = getInputValue("condition");
    bool condition = false;
    
    if (conditionValue.is<bool>()) {
        condition = conditionValue.get<bool>();
    } else {
        // 尝试转换为布尔值
        BlueprintValue convertedValue;
        if (DataTypeUtils::tryConvert(conditionValue, DataType::Boolean, convertedValue)) {
            condition = convertedValue.get<bool>();
        }
    }
    
    // 根据条件设置输出
    if (condition) {
        setOutputValue("true_exec", BlueprintValue(ExecutionToken(true)));
        setOutputValue("false_exec", BlueprintValue(ExecutionToken(false)));
        Logger::debug("Branch node: condition is true");
    } else {
        setOutputValue("true_exec", BlueprintValue(ExecutionToken(false)));
        setOutputValue("false_exec", BlueprintValue(ExecutionToken(true)));
        Logger::debug("Branch node: condition is false");
    }
    
    return result;
}

void BranchNode::initializePorts() {
    addInputPort("exec_in", "Execute", DataType::Execution, true);
    addInputPort("condition", "Condition", DataType::Boolean, true);
    
    addOutputPort("true_exec", "True", DataType::Execution);
    addOutputPort("false_exec", "False", DataType::Execution);
}

// LoopNode 实现

LoopNode::LoopNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Loop) {
}

std::unique_ptr<BaseNode> LoopNode::clone() const {
    auto cloned = std::make_unique<LoopNode>();
    cloned->m_loopCount = m_loopCount;
    return cloned;
}

void LoopNode::setLoopCount(int count) {
    m_loopCount = std::max(1, count);
}

NodeExecutionResult LoopNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 获取循环次数
    BlueprintValue countValue = getInputValue("count");
    if (countValue.is<int>()) {
        m_loopCount = std::max(1, countValue.get<int>());
    }
    
    // 设置当前索引输出
    setOutputValue("index", BlueprintValue(m_currentIndex));
    
    // 检查是否继续循环
    if (m_currentIndex < m_loopCount) {
        setOutputValue("loop_body", BlueprintValue(ExecutionToken(true)));
        setOutputValue("completed", BlueprintValue(ExecutionToken(false)));
        m_currentIndex++;
        Logger::debug("Loop node: iteration " + std::to_string(m_currentIndex) + "/" + std::to_string(m_loopCount));
    } else {
        setOutputValue("loop_body", BlueprintValue(ExecutionToken(false)));
        setOutputValue("completed", BlueprintValue(ExecutionToken(true)));
        Logger::debug("Loop node: completed");
    }
    
    return result;
}

void LoopNode::initializePorts() {
    addInputPort("exec_in", "Execute", DataType::Execution, true);
    addInputPort("count", "Count", DataType::Integer, false);
    
    addOutputPort("loop_body", "Loop Body", DataType::Execution);
    addOutputPort("completed", "Completed", DataType::Execution);
    addOutputPort("index", "Index", DataType::Integer);
}

void LoopNode::reset() {
    BaseNode::reset();
    m_currentIndex = 0;
}

// DelayNode 实现

DelayNode::DelayNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Delay) {
}

std::unique_ptr<BaseNode> DelayNode::clone() const {
    auto cloned = std::make_unique<DelayNode>();
    cloned->m_delayTime = m_delayTime;
    return cloned;
}

void DelayNode::setDelayTime(float seconds) {
    m_delayTime = std::max(0.0f, seconds);
}

NodeExecutionResult DelayNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 获取延迟时间
    BlueprintValue delayValue = getInputValue("delay");
    if (delayValue.is<float>()) {
        m_delayTime = std::max(0.0f, delayValue.get<float>());
    }
    
    // 执行延迟
    if (m_delayTime > 0.0f) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(m_delayTime * 1000)));
        Logger::debug("Delay node: waited " + std::to_string(m_delayTime) + " seconds");
    }
    
    // 设置输出
    setOutputValue("exec_out", BlueprintValue(ExecutionToken(true)));
    
    return result;
}

void DelayNode::initializePorts() {
    addInputPort("exec_in", "Execute", DataType::Execution, true);
    addInputPort("delay", "Delay (seconds)", DataType::Float, false);
    
    addOutputPort("exec_out", "Execute", DataType::Execution);
}

// SequenceNode 实现

SequenceNode::SequenceNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Sequence");
}

std::unique_ptr<BaseNode> SequenceNode::clone() const {
    auto cloned = std::make_unique<SequenceNode>();
    cloned->m_outputCount = m_outputCount;
    return cloned;
}

void SequenceNode::setOutputCount(int count) {
    m_outputCount = std::max(1, count);
    // 重新初始化端口
    // 注意：这里简化处理，实际应用中可能需要更复杂的端口管理
}

NodeExecutionResult SequenceNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 按顺序激活所有输出
    for (int i = 0; i < m_outputCount; ++i) {
        std::string portId = "exec_out_" + std::to_string(i);
        setOutputValue(portId, BlueprintValue(ExecutionToken(true)));
    }
    
    Logger::debug("Sequence node: activated " + std::to_string(m_outputCount) + " outputs");
    return result;
}

void SequenceNode::initializePorts() {
    addInputPort("exec_in", "Execute", DataType::Execution, true);
    
    for (int i = 0; i < m_outputCount; ++i) {
        std::string portId = "exec_out_" + std::to_string(i);
        std::string portName = "Then " + std::to_string(i);
        addOutputPort(portId, portName, DataType::Execution);
    }
}

// GateNode 实现

GateNode::GateNode(const std::string& id) 
    : BaseNode(id.empty() ? NodeUtils::generateNodeId() : id, NodeType::Custom) {
    setName("Gate");
}

std::unique_ptr<BaseNode> GateNode::clone() const {
    auto cloned = std::make_unique<GateNode>();
    cloned->m_isOpen = m_isOpen;
    return cloned;
}

void GateNode::setGateOpen(bool open) {
    m_isOpen = open;
}

NodeExecutionResult GateNode::executeInternal(ExecutionContext& context) {
    NodeExecutionResult result;
    result.success = true;
    
    // 获取门的状态
    BlueprintValue openValue = getInputValue("open");
    if (openValue.is<bool>()) {
        m_isOpen = openValue.get<bool>();
    }
    
    // 根据门的状态决定是否通过执行流
    if (m_isOpen) {
        setOutputValue("exec_out", BlueprintValue(ExecutionToken(true)));
        Logger::debug("Gate node: gate is open, execution continues");
    } else {
        setOutputValue("exec_out", BlueprintValue(ExecutionToken(false)));
        Logger::debug("Gate node: gate is closed, execution blocked");
    }
    
    return result;
}

void GateNode::initializePorts() {
    addInputPort("exec_in", "Execute", DataType::Execution, true);
    addInputPort("open", "Open", DataType::Boolean, false);
    
    addOutputPort("exec_out", "Execute", DataType::Execution);
}

} // namespace blueprint
} // namespace core
} // namespace oneday
