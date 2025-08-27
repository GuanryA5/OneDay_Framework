#pragma once

#include <memory>
#include <string>
#include <vector>

namespace oneday {
namespace core {
namespace blueprint {

class BlueprintGraph;

/**
 * @brief 执行结果结构
 */
struct ExecutionResult {
    bool success = false;        ///< 是否执行成功
    std::string message;         ///< 执行消息
    std::string errorDetails;    ///< 错误详情
    double executionTime = 0.0;  ///< 执行时间（毫秒）
    int nodesExecuted = 0;       ///< 已执行节点数
    int totalNodes = 0;          ///< 总节点数
};

/**
 * @brief 蓝图脚本执行引擎
 * 负责解析、编译和执行蓝图图表
 */
class Engine {
  public:
    Engine();
    ~Engine();

    /**
     * @brief 执行蓝图图表
     * @param graph 要执行的蓝图图表
     * @return 执行结果
     */
    ExecutionResult executeGraph(const BlueprintGraph& graph);

    /**
     * @brief 验证图表有效性
     * @param graph 要验证的图表
     * @return 验证结果
     */
    bool validateGraph(const BlueprintGraph& graph);

    /**
     * @brief 暂停执行
     */
    void pauseExecution();

    /**
     * @brief 恢复执行
     */
    void resumeExecution();

  private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

}  // namespace blueprint
}  // namespace core
}  // namespace oneday