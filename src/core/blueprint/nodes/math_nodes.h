#pragma once

#include "base_node.h"

namespace oneday {
namespace core {
namespace blueprint {

/**
 * @brief 加法节点
 */
class AddNode : public BaseNode {
public:
    explicit AddNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 减法节点
 */
class SubtractNode : public BaseNode {
public:
    explicit SubtractNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 乘法节点
 */
class MultiplyNode : public BaseNode {
public:
    explicit MultiplyNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 除法节点
 */
class DivideNode : public BaseNode {
public:
    explicit DivideNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 取模节点
 */
class ModuloNode : public BaseNode {
public:
    explicit ModuloNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 幂运算节点
 */
class PowerNode : public BaseNode {
public:
    explicit PowerNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 平方根节点
 */
class SqrtNode : public BaseNode {
public:
    explicit SqrtNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 绝对值节点
 */
class AbsNode : public BaseNode {
public:
    explicit AbsNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 最小值节点
 */
class MinNode : public BaseNode {
public:
    explicit MinNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 最大值节点
 */
class MaxNode : public BaseNode {
public:
    explicit MaxNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 限制范围节点
 */
class ClampNode : public BaseNode {
public:
    explicit ClampNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 线性插值节点
 */
class LerpNode : public BaseNode {
public:
    explicit LerpNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 三角函数类型
 */
enum class TrigFunction {
    Sin,    ///< 正弦
    Cos,    ///< 余弦
    Tan,    ///< 正切
    Asin,   ///< 反正弦
    Acos,   ///< 反余弦
    Atan    ///< 反正切
};

/**
 * @brief 三角函数节点
 */
class TrigNode : public BaseNode {
public:
    explicit TrigNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置三角函数类型
     */
    void setFunction(TrigFunction func);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    TrigFunction m_function = TrigFunction::Sin;
};

/**
 * @brief 随机数节点
 */
class RandomNode : public BaseNode {
public:
    explicit RandomNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;
};

/**
 * @brief 向量数学节点基类
 */
class VectorMathNode : public BaseNode {
public:
    enum class VectorOperation {
        Add,        ///< 向量加法
        Subtract,   ///< 向量减法
        Multiply,   ///< 向量乘法（分量相乘）
        Dot,        ///< 点积
        Cross,      ///< 叉积（仅3D向量）
        Length,     ///< 向量长度
        Normalize,  ///< 向量归一化
        Distance    ///< 两点间距离
    };
    
    explicit VectorMathNode(const std::string& id = "");
    
    std::unique_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 设置向量操作类型
     */
    void setOperation(VectorOperation op);

protected:
    NodeExecutionResult executeInternal(ExecutionContext& context) override;
    void initializePorts() override;

private:
    VectorOperation m_operation = VectorOperation::Add;
};

} // namespace blueprint
} // namespace core
} // namespace oneday
