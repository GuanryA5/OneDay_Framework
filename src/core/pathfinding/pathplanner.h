#pragma once

#include "map.h"
#include "astar.h"
#include <memory>
#include <vector>
#include <chrono>

namespace oneday::pathfinding {

/**
 * @brief 路径查找算法接口
 */
class PathfindingAlgorithm {
public:
    virtual ~PathfindingAlgorithm() = default;
    
    /**
     * @brief 查找路径
     * @param start 起点
     * @param goal 终点
     * @param map 地图
     * @return 路径点列表
     */
    virtual std::vector<Point> findPath(const Point& start, const Point& goal, const Map& map) = 0;
    
    /**
     * @brief 平滑路径
     * @param path 原始路径
     * @param map 地图
     * @return 平滑后的路径
     */
    virtual std::vector<Point> smoothPath(const std::vector<Point>& path, const Map& map) = 0;
    
    /**
     * @brief 设置启发式权重
     * @param weight 权重
     */
    virtual void setHeuristicWeight(double weight) = 0;
    
    /**
     * @brief 获取启发式权重
     * @return 权重
     */
    virtual double getHeuristicWeight() const = 0;
    
    /**
     * @brief 获取上次路径查找统计信息
     * @return 统计信息
     */
    virtual PathfindingStats getLastPathfindingStats() const = 0;
};

/**
 * @brief 路径规划器
 * 
 * 高级路径规划接口，支持多种算法和路径优化
 */
class PathPlanner {
public:
    /**
     * @brief 构造函数
     */
    PathPlanner();
    
    /**
     * @brief 析构函数
     */
    ~PathPlanner();
    
    /**
     * @brief 查找从起点到终点的路径
     * @param start 起点
     * @param goal 终点
     * @param map 地图
     * @return 路径点列表
     */
    std::vector<Point> findPath(const Point& start, const Point& goal, const Map& map);
    
    /**
     * @brief 通过多个路径点查找路径
     * @param waypoints 路径点列表（至少2个点）
     * @param map 地图
     * @return 完整路径
     */
    std::vector<Point> findPathWithWaypoints(const std::vector<Point>& waypoints, const Map& map);
    
    /**
     * @brief 平滑路径
     * @param path 原始路径
     * @param map 地图
     * @return 平滑后的路径
     */
    std::vector<Point> smoothPath(const std::vector<Point>& path, const Map& map);
    
    /**
     * @brief 优化路径（移除不必要的点）
     * @param path 原始路径
     * @param map 地图
     * @return 优化后的路径
     */
    std::vector<Point> optimizePath(const std::vector<Point>& path, const Map& map);
    
    /**
     * @brief 验证路径是否有效
     * @param path 路径
     * @param map 地图
     * @return 是否有效
     */
    bool isPathValid(const std::vector<Point>& path, const Map& map) const;
    
    /**
     * @brief 计算路径长度
     * @param path 路径
     * @return 路径长度
     */
    double calculatePathLength(const std::vector<Point>& path) const;
    
    /**
     * @brief 查找最近的可行走点
     * @param point 目标点
     * @param map 地图
     * @param maxRadius 最大搜索半径
     * @return 最近的可行走点，如果未找到返回(-1, -1)
     */
    Point findNearestWalkablePoint(const Point& point, const Map& map, int maxRadius = 10) const;
    
    /**
     * @brief 查找附近的所有可行走点
     * @param point 目标点
     * @param map 地图
     * @param maxRadius 最大搜索半径
     * @return 可行走点列表
     */
    std::vector<Point> findNearestWalkablePoints(const Point& point, const Map& map, int maxRadius = 10) const;
    
    /**
     * @brief 设置路径查找算法
     * @param algorithm 算法实例
     */
    void setAlgorithm(std::unique_ptr<PathfindingAlgorithm> algorithm);
    
    /**
     * @brief 设置启发式函数权重
     * @param weight 权重值
     */
    void setHeuristicWeight(double weight);
    
    /**
     * @brief 获取启发式函数权重
     * @return 权重值
     */
    double getHeuristicWeight() const;
    
    /**
     * @brief 启用/禁用路径平滑
     * @param enable 是否启用
     */
    void enableSmoothing(bool enable);
    
    /**
     * @brief 检查路径平滑是否启用
     * @return 是否启用
     */
    bool isSmoothingEnabled() const;
    
    /**
     * @brief 获取上次路径查找的执行时间
     * @return 执行时间（毫秒）
     */
    double getLastExecutionTime() const;
    
    /**
     * @brief 获取上次路径查找的统计信息
     * @return 统计信息
     */
    PathfindingStats getLastStats() const;

private:
    /**
     * @brief 检查两点间的连接是否有效
     * @param from 起点
     * @param to 终点
     * @param map 地图
     * @return 是否有效
     */
    bool isConnectionValid(const Point& from, const Point& to, const Map& map) const;
    
    /**
     * @brief 检查两点间是否有直线视线
     * @param from 起点
     * @param to 终点
     * @param map 地图
     * @return 是否有视线
     */
    bool hasLineOfSight(const Point& from, const Point& to, const Map& map) const;

private:
    std::unique_ptr<PathfindingAlgorithm> m_algorithm; ///< 路径查找算法
    bool m_enableSmoothing = true;                     ///< 是否启用路径平滑
    double m_lastExecutionTime = 0.0;                  ///< 上次执行时间
};

} // namespace oneday::pathfinding
