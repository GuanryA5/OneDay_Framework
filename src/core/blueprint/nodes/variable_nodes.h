#pragma once

#include "base_node.h"

namespace oneday {
namespace core {
namespace blueprint {

/**
 * @brief 获取变量节点
 */
class GetVariableNode : public BaseNode {
public:
    explicit GetVariableNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置变量名
     */
    void setVariableName(const std::string& name);
    
    /**
     * @brief 获取变量名
     */
    const std::string& getVariableName() const { return m_variableName; }

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    std::string m_variableName;
};

/**
 * @brief 设置变量节点
 */
class SetVariableNode : public BaseNode {
public:
    explicit SetVariableNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置变量名
     */
    void setVariableName(const std::string& name);
    
    /**
     * @brief 获取变量名
     */
    const std::string& getVariableName() const { return m_variableName; }

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    std::string m_variableName;
};

/**
 * @brief 增量变量节点 - 对数值变量进行增量操作
 */
class IncrementVariableNode : public BaseNode {
public:
    explicit IncrementVariableNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置变量名
     */
    void setVariableName(const std::string& name);
    
    /**
     * @brief 获取变量名
     */
    const std::string& getVariableName() const { return m_variableName; }

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    std::string m_variableName;
};

/**
 * @brief 变量存在检查节点
 */
class VariableExistsNode : public BaseNode {
public:
    explicit VariableExistsNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置变量名
     */
    void setVariableName(const std::string& name);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    std::string m_variableName;
};

/**
 * @brief 删除变量节点
 */
class DeleteVariableNode : public BaseNode {
public:
    explicit DeleteVariableNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置变量名
     */
    void setVariableName(const std::string& name);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    std::string m_variableName;
};

/**
 * @brief 获取所有变量节点 - 返回所有变量名的数组
 */
class GetAllVariablesNode : public BaseNode {
public:
    explicit GetAllVariablesNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 变量类型获取节点
 */
class GetVariableTypeNode : public BaseNode {
public:
    explicit GetVariableTypeNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置变量名
     */
    void setVariableName(const std::string& name);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    std::string m_variableName;
};

/**
 * @brief 局部变量节点 - 在当前执行上下文中创建临时变量
 */
class LocalVariableNode : public BaseNode {
public:
    explicit LocalVariableNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置变量名
     */
    void setVariableName(const std::string& name);
    
    /**
     * @brief 设置初始值
     */
    void setInitialValue(const BlueprintValue& value);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    std::string m_variableName;
    BlueprintValue m_initialValue;
};

/**
 * @brief 数组变量操作节点
 */
class ArrayVariableNode : public BaseNode {
public:
    enum class ArrayOperation {
        Get,        ///< 获取数组元素
        Set,        ///< 设置数组元素
        Add,        ///< 添加元素到数组末尾
        Insert,     ///< 在指定位置插入元素
        Remove,     ///< 移除指定索引的元素
        Clear,      ///< 清空数组
        Length      ///< 获取数组长度
    };
    
    explicit ArrayVariableNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置数组操作类型
     */
    void setOperation(ArrayOperation op);
    
    /**
     * @brief 设置变量名
     */
    void setVariableName(const std::string& name);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    ArrayOperation m_operation = ArrayOperation::Get;
    std::string m_variableName;
};

} // namespace blueprint
} // namespace core
} // namespace oneday
