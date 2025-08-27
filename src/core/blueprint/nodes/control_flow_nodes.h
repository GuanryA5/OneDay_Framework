#pragma once

#include "base_node.h"

namespace oneday {
namespace core {
namespace blueprint {

// 前向声明
class ExecutionContext;

/**
 * @brief 开始节点 - 蓝图执行的入口点
 */
class StartNode : public BaseNode {
public:
    explicit StartNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 结束节点 - 蓝图执行的出口点
 */
class EndNode : public BaseNode {
public:
    explicit EndNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 分支节点 - 根据条件选择执行路径
 */
class BranchNode : public BaseNode {
public:
    explicit BranchNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 循环节点 - 重复执行指定次数
 */
class LoopNode : public BaseNode {
public:
    explicit LoopNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置循环次数
     */
    void setLoopCount(int count);
    
    /**
     * @brief 获取当前循环索引
     */
    int getCurrentIndex() const { return m_currentIndex; }

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
    void reset() override;

private:
    int m_currentIndex = 0;     ///< 当前循环索引
    int m_loopCount = 1;        ///< 循环次数
};

/**
 * @brief 延迟节点 - 延迟指定时间后继续执行
 */
class DelayNode : public BaseNode {
public:
    explicit DelayNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置延迟时间（秒）
     */
    void setDelayTime(float seconds);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    float m_delayTime = 1.0f;   ///< 延迟时间（秒）
};

/**
 * @brief 序列节点 - 按顺序执行多个输出
 */
class SequenceNode : public BaseNode {
public:
    explicit SequenceNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置输出数量
     */
    void setOutputCount(int count);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    int m_outputCount = 2;      ///< 输出数量
};

/**
 * @brief 门节点 - 根据条件控制执行流的通过
 */
class GateNode : public BaseNode {
public:
    explicit GateNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置门的状态
     */
    void setGateOpen(bool open);
    
    /**
     * @brief 检查门是否打开
     */
    bool isGateOpen() const { return m_isOpen; }

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    bool m_isOpen = true;       ///< 门是否打开
};

/**
 * @brief 多路选择器节点 - 根据索引选择执行路径
 */
class MultiGateNode : public BaseNode {
public:
    explicit MultiGateNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置输出数量
     */
    void setOutputCount(int count);
    
    /**
     * @brief 设置当前选择的索引
     */
    void setSelectedIndex(int index);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    int m_outputCount = 2;      ///< 输出数量
    int m_selectedIndex = 0;    ///< 当前选择的索引
};

/**
 * @brief 翻转门节点 - 每次执行时切换输出
 */
class FlipFlopNode : public BaseNode {
public:
    explicit FlipFlopNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 重置翻转状态
     */
    void resetFlipFlop();

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
    void reset() override;

private:
    bool m_flipState = false;   ///< 翻转状态
};

} // namespace blueprint
} // namespace core
} // namespace oneday
