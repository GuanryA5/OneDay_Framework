#include "execution_context.h"
#include "graph.h"
#include "../common/logger.h"
#include <algorithm>

using oneday::core::Logger;

namespace oneday {
namespace core {
namespace blueprint {

ExecutionContext::ExecutionContext(BlueprintGraph* graph) 
    : m_graph(graph)
    , m_state(ExecutionState::Idle)
    , m_pauseRequested(false)
    , m_stopRequested(false) {
    
    // 创建全局作用域
    m_variableScopes.push_back(std::map<std::string, BlueprintValue>());
    
    Logger::debug("ExecutionContext created");
}

void ExecutionContext::setGraph(BlueprintGraph* graph) {
    m_graph = graph;
}

void ExecutionContext::setState(ExecutionState state) {
    ExecutionState oldState = m_state;
    m_state = state;
    
    Logger::debug("ExecutionContext state changed from " + std::to_string(static_cast<int>(oldState)) + 
                 " to " + std::to_string(static_cast<int>(state)));
    
    // 触发状态变化回调
    if (m_stateChangeCallback) {
        m_stateChangeCallback(oldState, state);
    }
}

// 变量管理实现

void ExecutionContext::setVariable(const std::string& name, const BlueprintValue& value) {
    if (name.empty()) {
        Logger::warning("Attempted to set variable with empty name");
        return;
    }
    
    getCurrentScope()[name] = value;
    Logger::debug("Set variable: " + name + " = " + value.toString());
}

BlueprintValue ExecutionContext::getVariable(const std::string& name) const {
    if (name.empty()) {
        Logger::warning("Attempted to get variable with empty name");
        return BlueprintValue();
    }
    
    // 从当前作用域向上查找变量
    for (auto it = m_variableScopes.rbegin(); it != m_variableScopes.rend(); ++it) {
        auto varIt = it->find(name);
        if (varIt != it->end()) {
            return varIt->second;
        }
    }
    
    Logger::debug("Variable not found: " + name);
    return BlueprintValue();
}

bool ExecutionContext::hasVariable(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 从当前作用域向上查找变量
    for (auto it = m_variableScopes.rbegin(); it != m_variableScopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return true;
        }
    }
    
    return false;
}

bool ExecutionContext::deleteVariable(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    
    // 从当前作用域向上查找并删除变量
    for (auto it = m_variableScopes.rbegin(); it != m_variableScopes.rend(); ++it) {
        auto varIt = it->find(name);
        if (varIt != it->end()) {
            it->erase(varIt);
            Logger::debug("Deleted variable: " + name);
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> ExecutionContext::getAllVariableNames() const {
    std::vector<std::string> names;
    
    // 收集所有作用域中的变量名（避免重复）
    for (const auto& scope : m_variableScopes) {
        for (const auto& pair : scope) {
            if (std::find(names.begin(), names.end(), pair.first) == names.end()) {
                names.push_back(pair.first);
            }
        }
    }
    
    return names;
}

void ExecutionContext::clearVariables() {
    for (auto& scope : m_variableScopes) {
        scope.clear();
    }
    Logger::debug("Cleared all variables");
}

// 作用域管理实现

void ExecutionContext::pushScope() {
    m_variableScopes.push_back(std::map<std::string, BlueprintValue>());
    Logger::debug("Pushed new variable scope, depth: " + std::to_string(m_variableScopes.size()));
}

void ExecutionContext::popScope() {
    if (m_variableScopes.size() > 1) {  // 保留全局作用域
        m_variableScopes.pop_back();
        Logger::debug("Popped variable scope, depth: " + std::to_string(m_variableScopes.size()));
    } else {
        Logger::warning("Cannot pop global variable scope");
    }
}

// 执行控制实现

void ExecutionContext::requestPause() {
    m_pauseRequested = true;
    Logger::debug("Pause requested");
}

void ExecutionContext::requestStop() {
    m_stopRequested = true;
    Logger::debug("Stop requested");
}

void ExecutionContext::resetRequests() {
    m_pauseRequested = false;
    m_stopRequested = false;
    Logger::debug("Reset execution requests");
}

// 统计信息实现

void ExecutionContext::resetStats() {
    m_stats = ExecutionStats();
    Logger::debug("Reset execution statistics");
}

void ExecutionContext::updateNodeStats(bool success, double executionTime) {
    m_stats.executedNodes++;
    if (!success) {
        m_stats.errorNodes++;
    }
    m_stats.totalExecutionTime += executionTime;
}

void ExecutionContext::startExecution() {
    m_stats.startTime = std::chrono::steady_clock::now();
    setState(ExecutionState::Running);
    Logger::debug("Started execution timing");
}

void ExecutionContext::endExecution() {
    m_stats.endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(m_stats.endTime - m_stats.startTime);
    
    if (m_state == ExecutionState::Running) {
        setState(ExecutionState::Completed);
    }
    
    Logger::info("Execution completed in " + std::to_string(duration.count()) + "ms, " +
                std::to_string(m_stats.executedNodes) + " nodes executed, " +
                std::to_string(m_stats.errorNodes) + " errors");
}

// 错误处理实现

void ExecutionContext::setError(const std::string& error) {
    m_errorMessage = error;
    setState(ExecutionState::Error);
    Logger::error("Execution error: " + error);
}

void ExecutionContext::clearError() {
    m_errorMessage.clear();
    Logger::debug("Cleared execution error");
}

// 调试支持实现

void ExecutionContext::setBreakpoint(const std::string& nodeId, bool enabled) {
    if (nodeId.empty()) {
        return;
    }
    
    m_breakpoints[nodeId] = enabled;
    Logger::debug("Set breakpoint on node " + nodeId + ": " + (enabled ? "enabled" : "disabled"));
}

void ExecutionContext::removeBreakpoint(const std::string& nodeId) {
    auto it = m_breakpoints.find(nodeId);
    if (it != m_breakpoints.end()) {
        m_breakpoints.erase(it);
        Logger::debug("Removed breakpoint from node " + nodeId);
    }
}

bool ExecutionContext::hasBreakpoint(const std::string& nodeId) const {
    auto it = m_breakpoints.find(nodeId);
    return it != m_breakpoints.end() && it->second;
}

void ExecutionContext::clearBreakpoints() {
    m_breakpoints.clear();
    Logger::debug("Cleared all breakpoints");
}

std::vector<std::string> ExecutionContext::getBreakpoints() const {
    std::vector<std::string> breakpoints;
    for (const auto& pair : m_breakpoints) {
        if (pair.second) {  // 只返回启用的断点
            breakpoints.push_back(pair.first);
        }
    }
    return breakpoints;
}

// 事件回调实现

void ExecutionContext::setNodeExecutionCallback(NodeExecutionCallback callback) {
    m_nodeExecutionCallback = callback;
}

void ExecutionContext::onNodeExecuting(const std::string& nodeId, const BaseNode* node) {
    if (m_nodeExecutionCallback) {
        m_nodeExecutionCallback(nodeId, node);
    }
}

void ExecutionContext::setStateChangeCallback(StateChangeCallback callback) {
    m_stateChangeCallback = callback;
}

// 私有方法实现

std::map<std::string, BlueprintValue>& ExecutionContext::getCurrentScope() {
    if (m_variableScopes.empty()) {
        // 这不应该发生，但为了安全起见
        m_variableScopes.push_back(std::map<std::string, BlueprintValue>());
    }
    return m_variableScopes.back();
}

const std::map<std::string, BlueprintValue>& ExecutionContext::getCurrentScope() const {
    if (m_variableScopes.empty()) {
        // 返回静态空映射
        static const std::map<std::string, BlueprintValue> emptyScope;
        return emptyScope;
    }
    return m_variableScopes.back();
}

} // namespace blueprint
} // namespace core
} // namespace oneday
