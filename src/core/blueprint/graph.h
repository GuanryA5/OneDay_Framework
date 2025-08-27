#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "data_types.h"
#include "nodes/base_node.h"

namespace oneday {
namespace core {
namespace blueprint {

// 前向声明
class ExecutionContext;

/**
 * @brief 图表元数据
 */
struct GraphMetadata {
    std::string version = "1.0";                       ///< 版本号
    std::string author;                                ///< 作者
    std::string description;                           ///< 描述
    std::string createdTime;                           ///< 创建时间
    std::string modifiedTime;                          ///< 修改时间
    std::map<std::string, BlueprintValue> customData;  ///< 自定义数据
};

/**
 * @brief 图表验证结果
 */
struct GraphValidationResult {
    bool isValid = false;               ///< 是否有效
    std::vector<std::string> errors;    ///< 错误列表
    std::vector<std::string> warnings;  ///< 警告列表
};

/**
 * @brief 蓝图图表类 - 管理节点和连接关系
 */
class BlueprintGraph {
  public:
    /**
     * @brief 构造函数
     */
    explicit BlueprintGraph(const std::string& id = "");

    /**
     * @brief 析构函数
     */
    ~BlueprintGraph() = default;

    /**
     * @brief 获取图表ID
     */
    const std::string& getId() const {
        return m_id;
    }

    /**
     * @brief 设置图表ID
     */
    void setId(const std::string& id) {
        m_id = id;
    }

    /**
     * @brief 获取图表名称
     */
    const std::string& getName() const {
        return m_name;
    }

    /**
     * @brief 设置图表名称
     */
    void setName(const std::string& name) {
        m_name = name;
    }

    /**
     * @brief 获取图表元数据
     */
    const GraphMetadata& getMetadata() const {
        return m_metadata;
    }

    /**
     * @brief 设置图表元数据
     */
    void setMetadata(const GraphMetadata& metadata) {
        m_metadata = metadata;
    }

    // 节点管理

    /**
     * @brief 添加节点
     */
    bool addNode(std::unique_ptr<BaseNode> node);

    /**
     * @brief 移除节点
     */
    bool removeNode(const std::string& nodeId);

    /**
     * @brief 根据ID查找节点
     */
    BaseNode* findNode(const std::string& nodeId) const;

    /**
     * @brief 获取所有节点
     */
    const std::vector<std::unique_ptr<BaseNode>>& getNodes() const {
        return m_nodes;
    }

    /**
     * @brief 获取节点数量
     */
    size_t getNodeCount() const {
        return m_nodes.size();
    }

    /**
     * @brief 根据类型查找节点
     */
    std::vector<BaseNode*> findNodesByType(NodeType type) const;

    /**
     * @brief 清空所有节点
     */
    void clearNodes();

    // 连接管理

    /**
     * @brief 添加连接
     */
    bool addConnection(const NodeConnection& connection);

    /**
     * @brief 移除连接
     */
    bool removeConnection(const std::string& connectionId);

    /**
     * @brief 移除节点的所有连接
     */
    void removeNodeConnections(const std::string& nodeId);

    /**
     * @brief 根据ID查找连接
     */
    const NodeConnection* findConnection(const std::string& connectionId) const;

    /**
     * @brief 获取所有连接
     */
    const std::vector<NodeConnection>& getConnections() const {
        return m_connections;
    }

    /**
     * @brief 获取连接数量
     */
    size_t getConnectionCount() const {
        return m_connections.size();
    }

    /**
     * @brief 获取节点的输入连接
     */
    std::vector<NodeConnection> getInputConnections(const std::string& nodeId) const;

    /**
     * @brief 获取节点的输出连接
     */
    std::vector<NodeConnection> getOutputConnections(const std::string& nodeId) const;

    /**
     * @brief 检查连接是否有效
     */
    bool isValidConnection(const NodeConnection& connection) const;

    /**
     * @brief 清空所有连接
     */
    void clearConnections();

    // 图表变量管理

    /**
     * @brief 设置图表变量
     */
    void setVariable(const std::string& name, const BlueprintValue& value);

    /**
     * @brief 获取图表变量
     */
    BlueprintValue getVariable(const std::string& name) const;

    /**
     * @brief 检查图表变量是否存在
     */
    bool hasVariable(const std::string& name) const;

    /**
     * @brief 删除图表变量
     */
    bool deleteVariable(const std::string& name);

    /**
     * @brief 获取所有图表变量
     */
    const std::map<std::string, BlueprintValue>& getVariables() const {
        return m_variables;
    }

    /**
     * @brief 清空所有图表变量
     */
    void clearVariables();

    // 图表验证

    /**
     * @brief 验证图表
     */
    GraphValidationResult validate() const;

    /**
     * @brief 检查是否有循环依赖
     */
    bool hasCyclicDependency() const;

    /**
     * @brief 获取拓扑排序的节点列表
     */
    std::vector<BaseNode*> getTopologicalOrder() const;

    /**
     * @brief 查找开始节点
     */
    std::vector<BaseNode*> findStartNodes() const;

    /**
     * @brief 查找结束节点
     */
    std::vector<BaseNode*> findEndNodes() const;

    // 序列化

    /**
     * @brief 序列化图表数据
     */
    std::map<std::string, BlueprintValue> serialize() const;

    /**
     * @brief 反序列化图表数据
     */
    bool deserialize(const std::map<std::string, BlueprintValue>& data);

    /**
     * @brief 导出为JSON字符串
     */
    std::string exportToJson() const;

    /**
     * @brief 从JSON字符串导入
     */
    bool importFromJson(const std::string& json);

    // 图表操作

    /**
     * @brief 克隆图表
     */
    std::unique_ptr<BlueprintGraph> clone() const;

    /**
     * @brief 合并另一个图表
     */
    bool merge(const BlueprintGraph& other);

    /**
     * @brief 清空图表
     */
    void clear();

    /**
     * @brief 重置所有节点状态
     */
    void resetNodeStates();

    // 事件回调

    /**
     * @brief 节点添加回调类型
     */
    using NodeAddedCallback = std::function<void(BaseNode* node)>;

    /**
     * @brief 节点移除回调类型
     */
    using NodeRemovedCallback = std::function<void(const std::string& nodeId)>;

    /**
     * @brief 连接添加回调类型
     */
    using ConnectionAddedCallback = std::function<void(const NodeConnection& connection)>;

    /**
     * @brief 连接移除回调类型
     */
    using ConnectionRemovedCallback = std::function<void(const std::string& connectionId)>;

    /**
     * @brief 设置节点添加回调
     */
    void setNodeAddedCallback(NodeAddedCallback callback) {
        m_nodeAddedCallback = callback;
    }

    /**
     * @brief 设置节点移除回调
     */
    void setNodeRemovedCallback(NodeRemovedCallback callback) {
        m_nodeRemovedCallback = callback;
    }

    /**
     * @brief 设置连接添加回调
     */
    void setConnectionAddedCallback(ConnectionAddedCallback callback) {
        m_connectionAddedCallback = callback;
    }

    /**
     * @brief 设置连接移除回调
     */
    void setConnectionRemovedCallback(ConnectionRemovedCallback callback) {
        m_connectionRemovedCallback = callback;
    }

  private:
    std::string m_id;          ///< 图表唯一标识
    std::string m_name;        ///< 图表名称
    GraphMetadata m_metadata;  ///< 图表元数据

    std::vector<std::unique_ptr<BaseNode>> m_nodes;     ///< 节点列表
    std::vector<NodeConnection> m_connections;          ///< 连接列表
    std::map<std::string, BlueprintValue> m_variables;  ///< 图表变量

    // 事件回调
    NodeAddedCallback m_nodeAddedCallback;
    NodeRemovedCallback m_nodeRemovedCallback;
    ConnectionAddedCallback m_connectionAddedCallback;
    ConnectionRemovedCallback m_connectionRemovedCallback;

    /**
     * @brief 生成唯一的图表ID
     */
    static std::string generateGraphId();

    /**
     * @brief 深度优先搜索检查循环
     */
    bool dfsCheckCycle(const std::string& nodeId,
                       std::map<std::string, int>& visited,
                       std::map<std::string, int>& recursionStack) const;
};

}  // namespace blueprint
}  // namespace core
}  // namespace oneday