#include "astar.h"

#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_set>

#include "../common/logger.h"

using oneday::core::Logger;

namespace oneday::pathfinding {

AStar::AStar() {
    Logger::info("A* pathfinder initialized");
}

AStar::~AStar() {
    Logger::info("A* pathfinder destroyed");
}

std::vector<Point> AStar::findPath(const Point& start, const Point& goal, const Map& map) {
    Logger::info("Starting A* pathfinding from (" + std::to_string(start.x) + "," +
                 std::to_string(start.y) + ") to (" + std::to_string(goal.x) + "," +
                 std::to_string(goal.y) + ")");

    // 检查起点和终点是否有效
    if (!map.isValidPosition(start) || !map.isValidPosition(goal)) {
        Logger::error("Invalid start or goal position");
        return std::vector<Point>();
    }

    if (!map.isWalkable(start) || !map.isWalkable(goal)) {
        Logger::error("Start or goal position is not walkable");
        return std::vector<Point>();
    }

    // 如果起点就是终点
    if (start == goal) {
        return {start};
    }

    // 初始化数据结构
    std::priority_queue<Node, std::vector<Node>, NodeComparator> openSet;
    std::unordered_set<Point, PointHash> closedSet;
    std::unordered_map<Point, Node, PointHash> allNodes;

    // 创建起始节点
    Node startNode;
    startNode.position = start;
    startNode.gCost = 0;
    startNode.hCost = calculateHeuristic(start, goal);
    startNode.fCost = startNode.gCost + startNode.hCost;
    startNode.parent = Point{-1, -1};  // 无效父节点表示起点

    openSet.push(startNode);
    allNodes[start] = startNode;

    int iterations = 0;
    const int maxIterations = map.getWidth() * map.getHeight();

    while (!openSet.empty() && iterations < maxIterations) {
        iterations++;

        // 获取F值最小的节点
        Node current = openSet.top();
        openSet.pop();

        // 如果已经在关闭列表中，跳过
        if (closedSet.find(current.position) != closedSet.end()) {
            continue;
        }

        // 添加到关闭列表
        closedSet.insert(current.position);

        // 检查是否到达目标
        if (current.position == goal) {
            Logger::info("Path found after " + std::to_string(iterations) + " iterations");
            return reconstructPath(current, allNodes);
        }

        // 检查所有邻居
        std::vector<Point> neighbors = getNeighbors(current.position, map);

        for (const Point& neighbor : neighbors) {
            // 跳过已在关闭列表中的节点
            if (closedSet.find(neighbor) != closedSet.end()) {
                continue;
            }

            // 计算到邻居的G值
            double tentativeGCost = current.gCost + calculateDistance(current.position, neighbor);

            // 检查是否已经在开放列表中
            auto it = allNodes.find(neighbor);
            bool inOpenSet = (it != allNodes.end());

            if (!inOpenSet || tentativeGCost < it->second.gCost) {
                // 创建或更新邻居节点
                Node neighborNode;
                neighborNode.position = neighbor;
                neighborNode.gCost = tentativeGCost;
                neighborNode.hCost = calculateHeuristic(neighbor, goal);
                neighborNode.fCost = neighborNode.gCost + neighborNode.hCost;
                neighborNode.parent = current.position;

                allNodes[neighbor] = neighborNode;
                openSet.push(neighborNode);
            }
        }
    }

    Logger::warning("No path found after " + std::to_string(iterations) + " iterations");
    return std::vector<Point>();
}

std::vector<Point> AStar::getNeighbors(const Point& point, const Map& map) {
    std::vector<Point> neighbors;

    // 8方向移动（包括对角线）
    const std::vector<Point> directions = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    for (const Point& dir : directions) {
        Point neighbor = {point.x + dir.x, point.y + dir.y};

        if (map.isValidPosition(neighbor) && map.isWalkable(neighbor)) {
            // 对于对角线移动，检查是否被阻挡
            if (dir.x != 0 && dir.y != 0) {
                Point horizontal = {point.x + dir.x, point.y};
                Point vertical = {point.x, point.y + dir.y};

                // 如果水平或垂直方向被阻挡，则不能对角线移动
                if (!map.isWalkable(horizontal) || !map.isWalkable(vertical)) {
                    continue;
                }
            }

            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}

double AStar::calculateHeuristic(const Point& from, const Point& to) {
    // 使用欧几里得距离作为启发式函数
    double dx = static_cast<double>(to.x - from.x);
    double dy = static_cast<double>(to.y - from.y);
    return std::sqrt(dx * dx + dy * dy);
}

double AStar::calculateDistance(const Point& from, const Point& to) {
    // 实际移动距离
    double dx = static_cast<double>(to.x - from.x);
    double dy = static_cast<double>(to.y - from.y);

    // 对角线移动的距离是sqrt(2)，直线移动的距离是1
    if (dx != 0 && dy != 0) {
        return std::sqrt(2.0);  // 对角线移动
    } else {
        return 1.0;  // 直线移动
    }
}

std::vector<Point> AStar::reconstructPath(
    const Node& goalNode,
    const std::unordered_map<Point, Node, PointHash>& allNodes) {
    std::vector<Point> path;
    Point current = goalNode.position;

    while (current.x != -1 && current.y != -1) {
        path.push_back(current);

        auto it = allNodes.find(current);
        if (it != allNodes.end()) {
            current = it->second.parent;
        } else {
            break;
        }
    }

    // 反转路径，使其从起点到终点
    std::reverse(path.begin(), path.end());

    Logger::info("Path reconstructed with " + std::to_string(path.size()) + " points");
    return path;
}

void AStar::setHeuristicWeight(double weight) {
    m_heuristicWeight = std::max(1.0, weight);
    Logger::info("Heuristic weight set to " + std::to_string(m_heuristicWeight));
}

double AStar::getHeuristicWeight() const {
    return m_heuristicWeight;
}

std::vector<Point> AStar::smoothPath(const std::vector<Point>& path, const Map& map) {
    if (path.size() <= 2) {
        return path;
    }

    std::vector<Point> smoothedPath;
    smoothedPath.push_back(path[0]);  // 添加起点

    size_t current = 0;

    while (current < path.size() - 1) {
        size_t farthest = current + 1;

        // 找到从当前点能直接到达的最远点
        for (size_t i = current + 2; i < path.size(); ++i) {
            if (hasLineOfSight(path[current], path[i], map)) {
                farthest = i;
            } else {
                break;
            }
        }

        smoothedPath.push_back(path[farthest]);
        current = farthest;
    }

    Logger::info("Path smoothed from " + std::to_string(path.size()) + " to " +
                 std::to_string(smoothedPath.size()) + " points");

    return smoothedPath;
}

bool AStar::hasLineOfSight(const Point& from, const Point& to, const Map& map) {
    // 使用Bresenham直线算法检查视线
    int dx = std::abs(to.x - from.x);
    int dy = std::abs(to.y - from.y);
    int x = from.x;
    int y = from.y;

    int x_inc = (to.x > from.x) ? 1 : -1;
    int y_inc = (to.y > from.y) ? 1 : -1;

    int error = dx - dy;

    while (true) {
        Point current = {x, y};

        if (!map.isValidPosition(current) || !map.isWalkable(current)) {
            return false;
        }

        if (x == to.x && y == to.y) {
            break;
        }

        int error2 = 2 * error;

        if (error2 > -dy) {
            error -= dy;
            x += x_inc;
        }

        if (error2 < dx) {
            error += dx;
            y += y_inc;
        }
    }

    return true;
}

PathfindingStats AStar::getLastPathfindingStats() const {
    return m_lastStats;
}

}  // namespace oneday::pathfinding
