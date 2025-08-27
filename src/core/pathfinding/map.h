#pragma once

#include <vector>
#include <string>

namespace oneday::pathfinding {

/**
 * @brief 点结构
 */
struct Point {
    int x = 0;
    int y = 0;
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

/**
 * @brief 地图单元格类型
 */
enum class CellType {
    Walkable = 0,  ///< 可行走
    Obstacle = 1,  ///< 障碍物
    Start = 2,     ///< 起点
    Goal = 3,      ///< 终点
    Any = -1       ///< 任意类型（用于特殊操作）
};

/**
 * @brief 地图类
 * 
 * 表示2D网格地图，用于路径查找算法
 */
class Map {
public:
    /**
     * @brief 构造函数
     * @param width 地图宽度
     * @param height 地图高度
     */
    Map(int width = 10, int height = 10);
    
    /**
     * @brief 析构函数
     */
    ~Map();
    
    /**
     * @brief 检查位置是否有效
     * @param point 位置
     * @return 是否有效
     */
    bool isValidPosition(const Point& point) const;
    
    /**
     * @brief 检查位置是否可行走
     * @param point 位置
     * @return 是否可行走
     */
    bool isWalkable(const Point& point) const;
    
    /**
     * @brief 获取单元格类型
     * @param point 位置
     * @return 单元格类型
     */
    CellType getCellType(const Point& point) const;
    
    /**
     * @brief 设置单元格类型
     * @param point 位置
     * @param type 单元格类型
     */
    void setCellType(const Point& point, CellType type);
    
    /**
     * @brief 获取地图宽度
     * @return 宽度
     */
    int getWidth() const;
    
    /**
     * @brief 获取地图高度
     * @return 高度
     */
    int getHeight() const;
    
    /**
     * @brief 清空地图
     * @param fillType 填充类型
     */
    void clear(CellType fillType = CellType::Walkable);
    
    /**
     * @brief 调整地图大小
     * @param newWidth 新宽度
     * @param newHeight 新高度
     * @param fillType 新区域填充类型
     */
    void resize(int newWidth, int newHeight, CellType fillType = CellType::Walkable);
    
    /**
     * @brief 设置矩形区域
     * @param topLeft 左上角
     * @param bottomRight 右下角
     * @param type 单元格类型
     */
    void setRectangle(const Point& topLeft, const Point& bottomRight, CellType type);
    
    /**
     * @brief 设置圆形区域
     * @param center 圆心
     * @param radius 半径
     * @param type 单元格类型
     */
    void setCircle(const Point& center, int radius, CellType type);
    
    /**
     * @brief 设置直线
     * @param start 起点
     * @param end 终点
     * @param type 单元格类型
     */
    void setLine(const Point& start, const Point& end, CellType type);
    
    /**
     * @brief 获取邻居位置
     * @param point 当前位置
     * @param includeDiagonal 是否包含对角线
     * @return 邻居位置列表
     */
    std::vector<Point> getNeighbors(const Point& point, bool includeDiagonal = true) const;
    
    /**
     * @brief 获取可行走的邻居位置
     * @param point 当前位置
     * @param includeDiagonal 是否包含对角线
     * @return 可行走的邻居位置列表
     */
    std::vector<Point> getWalkableNeighbors(const Point& point, bool includeDiagonal = true) const;
    
    /**
     * @brief 从文件加载地图
     * @param filename 文件名
     * @return 是否加载成功
     */
    bool loadFromFile(const std::string& filename);
    
    /**
     * @brief 保存地图到文件
     * @param filename 文件名
     * @return 是否保存成功
     */
    bool saveToFile(const std::string& filename) const;
    
    /**
     * @brief 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const;
    
    /**
     * @brief 打印地图到控制台
     */
    void printToConsole() const;
    
    /**
     * @brief 统计指定类型的单元格数量
     * @param type 单元格类型
     * @return 数量
     */
    int countCellsOfType(CellType type) const;
    
    /**
     * @brief 查找指定类型的所有单元格
     * @param type 单元格类型
     * @return 位置列表
     */
    std::vector<Point> findCellsOfType(CellType type) const;
    
    /**
     * @brief 查找指定类型的第一个单元格
     * @param type 单元格类型
     * @return 位置，如果未找到返回(-1, -1)
     */
    Point findFirstCellOfType(CellType type) const;
    
    /**
     * @brief 洪水填充
     * @param start 起始位置
     * @param newType 新类型
     * @param targetType 目标类型（Any表示使用起始位置的类型）
     */
    void floodFill(const Point& start, CellType newType, CellType targetType = CellType::Any);

private:
    int m_width;                    ///< 地图宽度
    int m_height;                   ///< 地图高度
    std::vector<CellType> m_data;   ///< 地图数据
};

} // namespace oneday::pathfinding
