#include "pathplanner.h"
#include "../common/logger.h"

using oneday::core::Logger;
#include "astar.h"

namespace oneday::pathfinding {

PathPlanner::PathPlanner() 
    : m_algorithm(std::make_unique<AStar>())
{
    Logger::info("PathPlanner initialized with A* algorithm");
}

PathPlanner::~PathPlanner() {
    Logger::info("PathPlanner destroyed");
}

std::vector<Point> PathPlanner::findPath(const Point& start, const Point& goal, const Map& map) {
    if (!m_algorithm) {
        Logger::error("No pathfinding algorithm set");
        return std::vector<Point>();
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<Point> path = m_algorithm->findPath(start, goal, map);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    m_lastExecutionTime = duration.count() / 1000.0; // 转换为毫�?    
    if (!path.empty()) {
        Logger::info("Path found with " + std::to_string(path.size()) + " points in " + 
                    std::to_string(m_lastExecutionTime) + " ms");
        
        if (m_enableSmoothing) {
            path = smoothPath(path, map);
        }
    } else {
        Logger::warning("No path found in " + std::to_string(m_lastExecutionTime) + " ms");
    }
    
    return path;
}

std::vector<Point> PathPlanner::findPathWithWaypoints(const std::vector<Point>& waypoints, const Map& map) {
    if (waypoints.size() < 2) {
        Logger::error("At least 2 waypoints required");
        return std::vector<Point>();
    }
    
    std::vector<Point> fullPath;
    
    for (size_t i = 0; i < waypoints.size() - 1; ++i) {
        std::vector<Point> segment = findPath(waypoints[i], waypoints[i + 1], map);
        
        if (segment.empty()) {
            Logger::error("Failed to find path between waypoints " + std::to_string(i) + 
                         " and " + std::to_string(i + 1));
            return std::vector<Point>();
        }
        
        // 避免重复添加连接�?        if (i > 0 && !fullPath.empty()) {
            segment.erase(segment.begin());
        }
        
        fullPath.insert(fullPath.end(), segment.begin(), segment.end());
    }
    
    Logger::info("Multi-waypoint path found with " + std::to_string(fullPath.size()) + " total points");
    return fullPath;
}

std::vector<Point> PathPlanner::smoothPath(const std::vector<Point>& path, const Map& map) {
    if (!m_algorithm) {
        return path;
    }
    
    return m_algorithm->smoothPath(path, map);
}

bool PathPlanner::isPathValid(const std::vector<Point>& path, const Map& map) const {
    if (path.empty()) {
        return false;
    }
    
    // 检查所有点是否可行�?    for (const Point& point : path) {
        if (!map.isWalkable(point)) {
            Logger::warning("Path contains non-walkable point at (" + 
                           std::to_string(point.x) + "," + std::to_string(point.y) + ")");
            return false;
        }
    }
    
    // 检查相邻点之间的连接是否有�?    for (size_t i = 0; i < path.size() - 1; ++i) {
        if (!isConnectionValid(path[i], path[i + 1], map)) {
            Logger::warning("Invalid connection between points " + std::to_string(i) + 
                           " and " + std::to_string(i + 1));
            return false;
        }
    }
    
    return true;
}

bool PathPlanner::isConnectionValid(const Point& from, const Point& to, const Map& map) const {
    // 检查两点之间的距离是否合理（最多对角线移动�?    int dx = std::abs(to.x - from.x);
    int dy = std::abs(to.y - from.y);
    
    if (dx > 1 || dy > 1) {
        // 距离太远，需要检查中间路�?        return hasLineOfSight(from, to, map);
    }
    
    // 对于对角线移动，检查是否被阻挡
    if (dx == 1 && dy == 1) {
        Point horizontal = {from.x + (to.x - from.x), from.y};
        Point vertical = {from.x, from.y + (to.y - from.y)};
        
        return map.isWalkable(horizontal) && map.isWalkable(vertical);
    }
    
    return true;
}

bool PathPlanner::hasLineOfSight(const Point& from, const Point& to, const Map& map) const {
    // 使用Bresenham直线算法检查视�?    int dx = std::abs(to.x - from.x);
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

double PathPlanner::calculatePathLength(const std::vector<Point>& path) const {
    if (path.size() < 2) {
        return 0.0;
    }
    
    double totalLength = 0.0;
    
    for (size_t i = 0; i < path.size() - 1; ++i) {
        double dx = static_cast<double>(path[i + 1].x - path[i].x);
        double dy = static_cast<double>(path[i + 1].y - path[i].y);
        totalLength += std::sqrt(dx * dx + dy * dy);
    }
    
    return totalLength;
}

std::vector<Point> PathPlanner::optimizePath(const std::vector<Point>& path, const Map& map) {
    if (path.size() <= 2) {
        return path;
    }
    
    std::vector<Point> optimized;
    optimized.push_back(path[0]); // 添加起点
    
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
        
        optimized.push_back(path[farthest]);
        current = farthest;
    }
    
    Logger::info("Path optimized from " + std::to_string(path.size()) + 
                " to " + std::to_string(optimized.size()) + " points");
    
    return optimized;
}

void PathPlanner::setAlgorithm(std::unique_ptr<PathfindingAlgorithm> algorithm) {
    m_algorithm = std::move(algorithm);
    Logger::info("Pathfinding algorithm changed");
}

void PathPlanner::setHeuristicWeight(double weight) {
    if (m_algorithm) {
        m_algorithm->setHeuristicWeight(weight);
        Logger::info("Heuristic weight set to " + std::to_string(weight));
    }
}

double PathPlanner::getHeuristicWeight() const {
    if (m_algorithm) {
        return m_algorithm->getHeuristicWeight();
    }
    return 1.0;
}

void PathPlanner::enableSmoothing(bool enable) {
    m_enableSmoothing = enable;
    Logger::info("Path smoothing " + std::string(enable ? "enabled" : "disabled"));
}

bool PathPlanner::isSmoothingEnabled() const {
    return m_enableSmoothing;
}

double PathPlanner::getLastExecutionTime() const {
    return m_lastExecutionTime;
}

PathfindingStats PathPlanner::getLastStats() const {
    if (m_algorithm) {
        return m_algorithm->getLastPathfindingStats();
    }
    return PathfindingStats{};
}

std::vector<Point> PathPlanner::findNearestWalkablePoints(const Point& point, const Map& map, int maxRadius) const {
    std::vector<Point> walkablePoints;
    
    if (map.isWalkable(point)) {
        walkablePoints.push_back(point);
        return walkablePoints;
    }
    
    // 螺旋搜索最近的可行走点
    for (int radius = 1; radius <= maxRadius; ++radius) {
        for (int dx = -radius; dx <= radius; ++dx) {
            for (int dy = -radius; dy <= radius; ++dy) {
                // 只检查当前半径圈上的�?                if (std::abs(dx) != radius && std::abs(dy) != radius) {
                    continue;
                }
                
                Point candidate = {point.x + dx, point.y + dy};
                
                if (map.isValidPosition(candidate) && map.isWalkable(candidate)) {
                    walkablePoints.push_back(candidate);
                }
            }
        }
        
        if (!walkablePoints.empty()) {
            break;
        }
    }
    
    Logger::info("Found " + std::to_string(walkablePoints.size()) + 
                " walkable points near (" + std::to_string(point.x) + "," + 
                std::to_string(point.y) + ")");
    
    return walkablePoints;
}

Point PathPlanner::findNearestWalkablePoint(const Point& point, const Map& map, int maxRadius) const {
    std::vector<Point> candidates = findNearestWalkablePoints(point, map, maxRadius);
    
    if (candidates.empty()) {
        return {-1, -1}; // 未找�?    }
    
    // 返回距离最近的�?    Point nearest = candidates[0];
    double minDistance = std::sqrt(std::pow(nearest.x - point.x, 2) + std::pow(nearest.y - point.y, 2));
    
    for (size_t i = 1; i < candidates.size(); ++i) {
        double distance = std::sqrt(std::pow(candidates[i].x - point.x, 2) + std::pow(candidates[i].y - point.y, 2));
        if (distance < minDistance) {
            minDistance = distance;
            nearest = candidates[i];
        }
    }
    
    return nearest;
}

} // namespace oneday::pathfinding
