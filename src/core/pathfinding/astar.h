#pragma once

#include <unordered_map>
#include <vector>

#include "map.h"

namespace oneday::pathfinding {

/**
 * @brief 路径查找统计信息
 */
struct PathfindingStats {
    int nodesExplored = 0;       ///< 探索的节点数
    int pathLength = 0;          ///< 路径长度
    double executionTime = 0.0;  ///< 执行时间（毫秒）
    bool pathFound = false;      ///< 是否找到路径
};

/**
 * @brief A*算法节点
 */
struct Node {
    Point position;      ///< 节点位置
    double gCost = 0.0;  ///< 从起点到当前节点的实际代价
    double hCost = 0.0;  ///< 从当前节点到终点的启发式代价
    double fCost = 0.0;  ///< 总代价 (gCost + hCost)
    Point parent;        ///< 父节点位置
};

/**
 * @brief 节点比较器（用于优先队列）
 */
struct NodeComparator {
    bool operator()(const Node& a, const Node& b) const {
        if (a.fCost == b.fCost) {
            return a.hCost > b.hCost;  // F值相同时，优先选择H值较小的
        }
        return a.fCost > b.fCost;  // 优先选择F值较小的
    }
};

/**
 * @brief Point哈希函数
 */
struct PointHash {
    std::size_t operator()(const Point& p) const {
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};

// 前向声明
class PathfindingAlgorithm;

/**
 * @brief A*路径查找算法实现
 */
class AStar : public PathfindingAlgorithm {
  public:
    /**
     * @brief 构造函数
     */
    AStar();

    /**
     * @brief 析构函数
     */
    ~AStar();

    /**
     * @brief 查找从起点到终点的路径
     * @param start 起点
     * @param goal 终点
     * @param map 地图
     * @return 路径点列表，如果没有找到路径则返回空列表
     */
    std::vector<Point> findPath(const Point& start, const Point& goal, const Map& map);

    /**
     * @brief 设置启发式函数权重
     * @param weight 权重值（>= 1.0）
     */
    void setHeuristicWeight(double weight);

    /**
     * @brief 获取启发式函数权重
     * @return 权重值
     */
    double getHeuristicWeight() const;

    /**
     * @brief 平滑路径（移除不必要的转折点）
     * @param path 原始路径
     * @param map 地图
     * @return 平滑后的路径
     */
    std::vector<Point> smoothPath(const std::vector<Point>& path, const Map& map);

    /**
     * @brief 获取上次路径查找的统计信息
     * @return 统计信息
     */
    PathfindingStats getLastPathfindingStats() const;

  private:
    /**
     * @brief 获取指定点的所有可行邻居
     * @param point 当前点
     * @param map 地图
     * @return 邻居点列表
     */
    std::vector<Point> getNeighbors(const Point& point, const Map& map);

    /**
     * @brief 计算启发式距离（欧几里得距离）
     * @param from 起点
     * @param to 终点
     * @return 启发式距离
     */
    double calculateHeuristic(const Point& from, const Point& to);

    /**
     * @brief 计算两点间的实际移动距离
     * @param from 起点
     * @param to 终点
     * @return 移动距离
     */
    double calculateDistance(const Point& from, const Point& to);

    /**
     * @brief 重构路径
     * @param goalNode 目标节点
     * @param allNodes 所有节点的映射
     * @return 重构的路径
     */
    std::vector<Point> reconstructPath(const Node& goalNode,
                                       const std::unordered_map<Point, Node, PointHash>& allNodes);

    /**
     * @brief 检查两点间是否有直线视线
     * @param from 起点
     * @param to 终点
     * @param map 地图
     * @return 是否有视线
     */
    bool hasLineOfSight(const Point& from, const Point& to, const Map& map);

  private:
    double m_heuristicWeight = 1.0;  ///< 启发式函数权重
    PathfindingStats m_lastStats;    ///< 上次查找的统计信息
};

}  // namespace oneday::pathfinding
