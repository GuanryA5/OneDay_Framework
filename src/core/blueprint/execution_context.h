#pragma once

#include "data_types.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>

namespace oneday {
namespace core {
namespace blueprint {

// 前向声明
class BaseNode;
class BlueprintGraph;

/**
 * @brief 执行状态枚举
 */
enum class ExecutionState {
    Idle,           ///< 空闲状态
    Running,        ///< 正在运行
    Paused,         ///< 已暂停
    Completed,      ///< 执行完成
    Error,          ///< 执行错误
    Cancelled       ///< 已取消
};

/**
 * @brief 执行统计信息
 */
struct ExecutionStats {
    int totalNodes = 0;             ///< 总节点数
    int executedNodes = 0;          ///< 已执行节点数
    int errorNodes = 0;             ///< 错误节点数
    double totalExecutionTime = 0.0; ///< 总执行时间（毫秒）
    std::chrono::steady_clock::time_point startTime; ///< 开始时间
    std::chrono::steady_clock::time_point endTime;   ///< 结束时间
};

/**
 * @brief 执行上下文 - 管理蓝图执行过程中的状态和数据
 */
class ExecutionContext {
public:
    /**
     * @brief 构造函数
     */
    explicit ExecutionContext(BlueprintGraph* graph = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~ExecutionContext() = default;
    
    /**
     * @brief 设置关联的蓝图图表
     */
    void setGraph(BlueprintGraph* graph);
    
    /**
     * @brief 获取关联的蓝图图表
     */
    BlueprintGraph* getGraph() const { return m_graph; }
    
    /**
     * @brief 获取执行状态
     */
    ExecutionState getState() const { return m_state; }
    
    /**
     * @brief 设置执行状态
     */
    void setState(ExecutionState state);
    
    /**
     * @brief 检查是否正在运行
     */
    bool isRunning() const { return m_state == ExecutionState::Running; }
    
    /**
     * @brief 检查是否已暂停
     */
    bool isPaused() const { return m_state == ExecutionState::Paused; }
    
    /**
     * @brief 检查是否已完成
     */
    bool isCompleted() const { return m_state == ExecutionState::Completed; }
    
    /**
     * @brief 检查是否有错误
     */
    bool hasError() const { return m_state == ExecutionState::Error; }
    
    // 变量管理
    
    /**
     * @brief 设置变量值
     */
    void setVariable(const std::string& name, const BlueprintValue& value);
    
    /**
     * @brief 获取变量值
     */
    BlueprintValue getVariable(const std::string& name) const;
    
    /**
     * @brief 检查变量是否存在
     */
    bool hasVariable(const std::string& name) const;
    
    /**
     * @brief 删除变量
     */
    bool deleteVariable(const std::string& name);
    
    /**
     * @brief 获取所有变量名
     */
    std::vector<std::string> getAllVariableNames() const;
    
    /**
     * @brief 清空所有变量
     */
    void clearVariables();
    
    // 局部变量管理（作用域相关）
    
    /**
     * @brief 推入新的变量作用域
     */
    void pushScope();
    
    /**
     * @brief 弹出当前变量作用域
     */
    void popScope();
    
    /**
     * @brief 获取当前作用域深度
     */
    int getScopeDepth() const { return static_cast<int>(m_variableScopes.size()); }
    
    // 执行控制
    
    /**
     * @brief 请求暂停执行
     */
    void requestPause();
    
    /**
     * @brief 请求停止执行
     */
    void requestStop();
    
    /**
     * @brief 检查是否请求暂停
     */
    bool isPauseRequested() const { return m_pauseRequested; }
    
    /**
     * @brief 检查是否请求停止
     */
    bool isStopRequested() const { return m_stopRequested; }
    
    /**
     * @brief 重置执行请求标志
     */
    void resetRequests();
    
    // 执行统计
    
    /**
     * @brief 获取执行统计信息
     */
    const ExecutionStats& getStats() const { return m_stats; }
    
    /**
     * @brief 重置统计信息
     */
    void resetStats();
    
    /**
     * @brief 更新节点执行统计
     */
    void updateNodeStats(bool success, double executionTime);
    
    /**
     * @brief 开始执行计时
     */
    void startExecution();
    
    /**
     * @brief 结束执行计时
     */
    void endExecution();
    
    // 错误处理
    
    /**
     * @brief 设置错误信息
     */
    void setError(const std::string& error);
    
    /**
     * @brief 获取错误信息
     */
    const std::string& getError() const { return m_errorMessage; }
    
    /**
     * @brief 检查是否有错误信息
     */
    bool hasErrorMessage() const { return !m_errorMessage.empty(); }
    
    /**
     * @brief 清除错误信息
     */
    void clearError();
    
    // 调试支持
    
    /**
     * @brief 设置断点节点
     */
    void setBreakpoint(const std::string& nodeId, bool enabled = true);
    
    /**
     * @brief 移除断点
     */
    void removeBreakpoint(const std::string& nodeId);
    
    /**
     * @brief 检查节点是否有断点
     */
    bool hasBreakpoint(const std::string& nodeId) const;
    
    /**
     * @brief 清除所有断点
     */
    void clearBreakpoints();
    
    /**
     * @brief 获取所有断点节点ID
     */
    std::vector<std::string> getBreakpoints() const;
    
    // 事件回调
    
    /**
     * @brief 节点执行前回调类型
     */
    using NodeExecutionCallback = std::function<void(const std::string& nodeId, const BaseNode* node)>;
    
    /**
     * @brief 设置节点执行前回调
     */
    void setNodeExecutionCallback(NodeExecutionCallback callback);
    
    /**
     * @brief 触发节点执行前回调
     */
    void onNodeExecuting(const std::string& nodeId, const BaseNode* node);
    
    /**
     * @brief 状态变化回调类型
     */
    using StateChangeCallback = std::function<void(ExecutionState oldState, ExecutionState newState)>;
    
    /**
     * @brief 设置状态变化回调
     */
    void setStateChangeCallback(StateChangeCallback callback);

private:
    BlueprintGraph* m_graph;                                    ///< 关联的蓝图图表
    ExecutionState m_state;                                     ///< 执行状态
    
    // 变量管理
    std::vector<std::map<std::string, BlueprintValue>> m_variableScopes; ///< 变量作用域栈
    
    // 执行控制
    bool m_pauseRequested;                                      ///< 暂停请求标志
    bool m_stopRequested;                                       ///< 停止请求标志
    
    // 统计信息
    ExecutionStats m_stats;                                     ///< 执行统计
    
    // 错误处理
    std::string m_errorMessage;                                 ///< 错误信息
    
    // 调试支持
    std::map<std::string, bool> m_breakpoints;                  ///< 断点映射
    
    // 事件回调
    NodeExecutionCallback m_nodeExecutionCallback;             ///< 节点执行回调
    StateChangeCallback m_stateChangeCallback;                 ///< 状态变化回调
    
    /**
     * @brief 获取当前变量作用域
     */
    std::map<std::string, BlueprintValue>& getCurrentScope();
    
    /**
     * @brief 获取当前变量作用域（只读）
     */
    const std::map<std::string, BlueprintValue>& getCurrentScope() const;
};

} // namespace blueprint
} // namespace core
} // namespace oneday
