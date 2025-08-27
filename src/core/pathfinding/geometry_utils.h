#pragma once

// Boost.Geometry 头文件
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/index/rtree.hpp>

// Eigen3 头文件 (用于高级数学运算)
#define EIGEN_DONT_USE_MKL
#include <Eigen/Dense>
#include <Eigen/Geometry>

// 标准库
#include <cmath>
#include <memory>
#include <vector>

namespace oneday::pathfinding {

// Boost.Geometry 命名空间别名
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// 基础几何类型定义 - 替代CGAL类型
using Point2D = bg::model::d2::point_xy<float>;
using Polygon = bg::model::polygon<Point2D>;
using LineString = bg::model::linestring<Point2D>;
using Box = bg::model::box<Point2D>;
using MultiPolygon = bg::model::multi_polygon<Polygon>;

// 空间索引类型
using RTreeValue = std::pair<Box, size_t>;
using RTree = bgi::rtree<RTreeValue, bgi::quadratic<16>>;

// Eigen类型别名 (用于高级数学运算)
using Vector2f = Eigen::Vector2f;
using Matrix2f = Eigen::Matrix2f;
using Transform2f = Eigen::Transform<float, 2, Eigen::Affine>;

/**
 * @brief 几何工具类 - 替代CGAL功能
 *
 * 使用Boost.Geometry和Eigen3提供高性能的几何计算功能
 * 专门针对游戏寻路场景优化
 */
class GeometryUtils {
  public:
    // === 基础几何运算 ===

    /**
     * @brief 检查点是否在多边形内
     * @param point 待检查的点
     * @param polygon 多边形
     * @return true 如果点在多边形内
     */
    static bool pointInPolygon(const Point2D& point, const Polygon& polygon);

    /**
     * @brief 检查两个多边形是否相交
     * @param a 第一个多边形
     * @param b 第二个多边形
     * @return true 如果多边形相交
     */
    static bool polygonsIntersect(const Polygon& a, const Polygon& b);

    /**
     * @brief 计算两点之间的距离
     * @param a 第一个点
     * @param b 第二个点
     * @return 两点间的欧几里得距离
     */
    static float distance(const Point2D& a, const Point2D& b);

    /**
     * @brief 检查线段是否与多边形相交
     * @param start 线段起点
     * @param end 线段终点
     * @param polygon 多边形
     * @return true 如果线段与多边形相交
     */
    static bool lineIntersectsPolygon(const Point2D& start,
                                      const Point2D& end,
                                      const Polygon& polygon);

    // === 多边形操作 ===

    /**
     * @brief 简化多边形 (Douglas-Peucker算法)
     * @param polygon 原始多边形
     * @param tolerance 简化容差
     * @return 简化后的多边形
     */
    static Polygon simplifyPolygon(const Polygon& polygon, float tolerance);

    /**
     * @brief 计算多边形面积
     * @param polygon 多边形
     * @return 多边形面积
     */
    static float polygonArea(const Polygon& polygon);

    /**
     * @brief 创建多边形缓冲区
     * @param polygon 原始多边形
     * @param distance 缓冲距离
     * @return 缓冲区多边形列表
     */
    static std::vector<Polygon> bufferPolygon(const Polygon& polygon, float distance);

    // === 几何体创建 ===

    /**
     * @brief 创建矩形多边形
     * @param x 左下角X坐标
     * @param y 左下角Y坐标
     * @param width 宽度
     * @param height 高度
     * @return 矩形多边形
     */
    static Polygon createRectangle(float x, float y, float width, float height);

    /**
     * @brief 创建圆形多边形
     * @param center 圆心
     * @param radius 半径
     * @param segments 分段数 (默认32)
     * @return 圆形多边形
     */
    static Polygon createCircle(const Point2D& center, float radius, int segments = 32);

    // === 边界框操作 ===

    /**
     * @brief 获取多边形的边界框
     * @param polygon 多边形
     * @return 边界框
     */
    static Box getBoundingBox(const Polygon& polygon);

    /**
     * @brief 检查点是否在边界框内
     * @param point 点
     * @param box 边界框
     * @return true 如果点在边界框内
     */
    static bool pointInBox(const Point2D& point, const Box& box);

    // === 坐标转换工具 ===

    /**
     * @brief Point2D转换为Eigen Vector2f
     * @param point Boost.Geometry点
     * @return Eigen向量
     */
    static Vector2f toEigen(const Point2D& point) {
        return Vector2f(point.x(), point.y());
    }

    /**
     * @brief Eigen Vector2f转换为Point2D
     * @param vec Eigen向量
     * @return Boost.Geometry点
     */
    static Point2D fromEigen(const Vector2f& vec) {
        return Point2D(vec.x(), vec.y());
    }

    // === 常用常量 ===
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float EPSILON = 1e-6f;
};

}  // namespace oneday::pathfinding
