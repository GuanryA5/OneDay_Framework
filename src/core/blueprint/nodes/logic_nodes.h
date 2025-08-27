#pragma once

#include "base_node.h"

namespace oneday {
namespace core {
namespace blueprint {

/**
 * @brief 逻辑与节点
 */
class AndNode : public BaseNode {
public:
    explicit AndNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 逻辑或节点
 */
class OrNode : public BaseNode {
public:
    explicit OrNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 逻辑非节点
 */
class NotNode : public BaseNode {
public:
    explicit NotNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 比较操作类型
 */
enum class CompareOperation {
    Equal,              ///< 等于
    NotEqual,           ///< 不等于
    Greater,            ///< 大于
    GreaterOrEqual,     ///< 大于等于
    Less,               ///< 小于
    LessOrEqual         ///< 小于等于
};

/**
 * @brief 比较节点
 */
class CompareNode : public BaseNode {
public:
    explicit CompareNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置比较操作类型
     */
    void setOperation(CompareOperation op);
    
    /**
     * @brief 获取比较操作类型
     */
    CompareOperation getOperation() const { return m_operation; }

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    CompareOperation m_operation = CompareOperation::Equal;
    
    /**
     * @brief 执行比较操作
     */
    bool performComparison(const BlueprintValue& a, const BlueprintValue& b) const;
};

/**
 * @brief 异或节点
 */
class XorNode : public BaseNode {
public:
    explicit XorNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 多输入逻辑与节点
 */
class MultiAndNode : public BaseNode {
public:
    explicit MultiAndNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置输入数量
     */
    void setInputCount(int count);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    int m_inputCount = 2;
};

/**
 * @brief 多输入逻辑或节点
 */
class MultiOrNode : public BaseNode {
public:
    explicit MultiOrNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置输入数量
     */
    void setInputCount(int count);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    int m_inputCount = 2;
};

/**
 * @brief 条件选择节点 - 根据条件选择两个值中的一个
 */
class SelectNode : public BaseNode {
public:
    explicit SelectNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 范围检查节点 - 检查值是否在指定范围内
 */
class InRangeNode : public BaseNode {
public:
    explicit InRangeNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 类型检查节点 - 检查值的数据类型
 */
class IsTypeNode : public BaseNode {
public:
    explicit IsTypeNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置要检查的类型
     */
    void setTargetType(DataType type);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    DataType m_targetType = DataType::None;
};

} // namespace blueprint
} // namespace core
} // namespace oneday
