#pragma once

#include "../data_types.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace oneday {
namespace core {
namespace blueprint {

// 前向声明
class ExecutionContext;
class BlueprintGraph;

/**
 * @brief 节点端口类型
 */
enum class PortType {
    Input,      ///< 输入端口
    Output      ///< 输出端口
};

/**
 * @brief 节点端口
 */
struct NodePort {
    std::string id;                 ///< 端口唯一标识
    std::string name;               ///< 端口显示名称
    DataType dataType;              ///< 数据类型
    PortType portType;              ///< 端口类型
    BlueprintValue defaultValue;    ///< 默认值
    bool isRequired = false;        ///< 是否必需连接
    bool isArray = false;           ///< 是否为数组端口
    
    NodePort() = default;
    NodePort(const std::string& id_, const std::string& name_, DataType type, PortType portType_)
        : id(id_), name(name_), dataType(type), portType(portType_) {}
};

/**
 * @brief 节点连接
 */
struct NodeConnection {
    std::string id;                 ///< 连接唯一标识
    std::string sourceNodeId;       ///< 源节点ID
    std::string sourcePortId;       ///< 源端口ID
    std::string targetNodeId;       ///< 目标节点ID
    std::string targetPortId;       ///< 目标端口ID
    
    NodeConnection() = default;
    NodeConnection(const std::string& sourceNode, const std::string& sourcePort,
                  const std::string& targetNode, const std::string& targetPort)
        : sourceNodeId(sourceNode), sourcePortId(sourcePort)
        , targetNodeId(targetNode), targetPortId(targetPort) {}
};

/**
 * @brief 节点执行状态
 */
enum class NodeState {
    Idle,           ///< 空闲状态
    Ready,          ///< 准备执行
    Executing,      ///< 正在执行
    Completed,      ///< 执行完成
    Error,          ///< 执行错误
    Disabled        ///< 已禁用
};

/**
 * @brief 节点执行结果
 */
struct NodeExecutionResult {
    bool success = false;           ///< 是否执行成功
    std::string errorMessage;       ///< 错误信息
    double executionTime = 0.0;     ///< 执行时间（毫秒）
    std::map<std::string, BlueprintValue> outputValues; ///< 输出值
};

/**
 * @brief 节点类型枚举
 */
enum class NodeType {
    // 控制流节点
    Start,          ///< 开始节点
    End,            ///< 结束节点
    Branch,         ///< 分支节点
    Loop,           ///< 循环节点
    Delay,          ///< 延迟节点
    
    // 逻辑运算节点
    And,            ///< 逻辑与
    Or,             ///< 逻辑或
    Not,            ///< 逻辑非
    Compare,        ///< 比较节点
    
    // 数学运算节点
    Add,            ///< 加法
    Subtract,       ///< 减法
    Multiply,       ///< 乘法
    Divide,         ///< 除法
    
    // 变量操作节点
    GetVariable,    ///< 获取变量
    SetVariable,    ///< 设置变量
    
    // 自定义节点
    Custom          ///< 自定义节点
};

/**
 * @brief 节点基类
 */
class BaseNode {
public:
    /**
     * @brief 构造函数
     */
    BaseNode(const std::string& id, NodeType type);
    
    /**
     * @brief 虚析构函数
     */
    virtual ~BaseNode() = default;
    
    /**
     * @brief 获取节点ID
     */
    const std::string& getId() const { return m_id; }
    
    /**
     * @brief 获取节点类型
     */
    NodeType getType() const { return m_type; }
    
    /**
     * @brief 获取节点名称
     */
    const std::string& getName() const { return m_name; }
    
    /**
     * @brief 设置节点名称
     */
    void setName(const std::string& name) { m_name = name; }
    
    /**
     * @brief 获取节点描述
     */
    const std::string& getDescription() const { return m_description; }
    
    /**
     * @brief 设置节点描述
     */
    void setDescription(const std::string& description) { m_description = description; }
    
    /**
     * @brief 获取节点状态
     */
    NodeState getState() const { return m_state; }
    
    /**
     * @brief 设置节点状态
     */
    void setState(NodeState state) { m_state = state; }
    
    /**
     * @brief 获取输入端口列表
     */
    const std::vector<NodePort>& getInputPorts() const { return m_inputPorts; }
    
    /**
     * @brief 获取输出端口列表
     */
    const std::vector<NodePort>& getOutputPorts() const { return m_outputPorts; }
    
    /**
     * @brief 根据ID查找输入端口
     */
    const NodePort* findInputPort(const std::string& portId) const;
    
    /**
     * @brief 根据ID查找输出端口
     */
    const NodePort* findOutputPort(const std::string& portId) const;
    
    /**
     * @brief 设置输入值
     */
    void setInputValue(const std::string& portId, const BlueprintValue& value);
    
    /**
     * @brief 获取输入值
     */
    BlueprintValue getInputValue(const std::string& portId) const;
    
    /**
     * @brief 获取输出值
     */
    BlueprintValue getOutputValue(const std::string& portId) const;
    
    /**
     * @brief 检查是否可以执行
     */
    virtual bool canExecute() const;
    
    /**
     * @brief 执行节点
     */
    virtual NodeExecutionResult execute(ExecutionContext& context);
    
    /**
     * @brief 重置节点状态
     */
    virtual void reset();
    
    /**
     * @brief 验证节点配置
     */
    virtual bool validate(std::string& errorMessage) const;
    
    /**
     * @brief 克隆节点
     */
    virtual std::unique_ptr<BaseNode> clone() const = 0;
    
    /**
     * @brief 序列化节点数据
     */
    virtual std::map<std::string, BlueprintValue> serialize() const;
    
    /**
     * @brief 反序列化节点数据
     */
    virtual bool deserialize(const std::map<std::string, BlueprintValue>& data);

protected:
    /**
     * @brief 添加输入端口
     */
    void addInputPort(const std::string& id, const std::string& name, DataType type, bool required = false);
    
    /**
     * @brief 添加输出端口
     */
    void addOutputPort(const std::string& id, const std::string& name, DataType type);
    
    /**
     * @brief 设置输出值
     */
    void setOutputValue(const std::string& portId, const BlueprintValue& value);
    
    /**
     * @brief 执行节点逻辑（子类实现）
     */
    virtual NodeExecutionResult executeInternal(ExecutionContext& context) = 0;
    
    /**
     * @brief 初始化端口（子类实现）
     */
    virtual void initializePorts() = 0;

private:
    std::string m_id;                                   ///< 节点唯一标识
    NodeType m_type;                                    ///< 节点类型
    std::string m_name;                                 ///< 节点名称
    std::string m_description;                          ///< 节点描述
    NodeState m_state;                                  ///< 节点状态
    
    std::vector<NodePort> m_inputPorts;                 ///< 输入端口列表
    std::vector<NodePort> m_outputPorts;                ///< 输出端口列表
    
    std::map<std::string, BlueprintValue> m_inputValues;  ///< 输入值缓存
    std::map<std::string, BlueprintValue> m_outputValues; ///< 输出值缓存
};

/**
 * @brief 节点工具类
 */
class NodeUtils {
public:
    /**
     * @brief 获取节点类型的字符串名称
     */
    static std::string getNodeTypeName(NodeType type);
    
    /**
     * @brief 获取节点类型的颜色
     */
    static Color getNodeTypeColor(NodeType type);
    
    /**
     * @brief 生成唯一的节点ID
     */
    static std::string generateNodeId();
    
    /**
     * @brief 生成唯一的连接ID
     */
    static std::string generateConnectionId();
    
    /**
     * @brief 验证连接是否有效
     */
    static bool isValidConnection(const NodePort& sourcePort, const NodePort& targetPort);

private:
    static const std::map<NodeType, std::string> s_nodeTypeNames;
    static const std::map<NodeType, Color> s_nodeTypeColors;
};

} // namespace blueprint
} // namespace core
} // namespace oneday
