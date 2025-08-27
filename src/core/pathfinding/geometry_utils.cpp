#include "geometry_utils.h"
#include "../common/logger.h"

namespace oneday::pathfinding {

// 静态成员函数实现
bool GeometryUtils::pointInPolygon(const Point2D& point, const Polygon& polygon) {
    try {
        return bg::within(point, polygon);
    } catch (const std::exception& e) {
        Logger::error("Error in pointInPolygon: " + std::string(e.what()));
        return false;
    }
}

bool GeometryUtils::polygonsIntersect(const Polygon& a, const Polygon& b) {
    try {
        return bg::intersects(a, b);
    } catch (const std::exception& e) {
        Logger::error("Error in polygonsIntersect: " + std::string(e.what()));
        return false;
    }
}

float GeometryUtils::distance(const Point2D& a, const Point2D& b) {
    try {
        return static_cast<float>(bg::distance(a, b));
    } catch (const std::exception& e) {
        Logger::error("Error in distance calculation: " + std::string(e.what()));
        return 0.0f;
    }
}

bool GeometryUtils::lineIntersectsPolygon(const Point2D& start, const Point2D& end, 
                                         const Polygon& polygon) {
    try {
        LineString line;
        bg::append(line, start);
        bg::append(line, end);
        return bg::intersects(line, polygon);
    } catch (const std::exception& e) {
        Logger::error("Error in lineIntersectsPolygon: " + std::string(e.what()));
        return true; // 安全起见，假设相交
    }
}

Polygon GeometryUtils::simplifyPolygon(const Polygon& polygon, float tolerance) {
    try {
        Polygon simplified;
        bg::simplify(polygon, simplified, static_cast<double>(tolerance));
        return simplified;
    } catch (const std::exception& e) {
        Logger::error("Error in simplifyPolygon: " + std::string(e.what()));
        return polygon; // 返回原始多边形
    }
}

float GeometryUtils::polygonArea(const Polygon& polygon) {
    try {
        return static_cast<float>(bg::area(polygon));
    } catch (const std::exception& e) {
        Logger::error("Error in polygonArea: " + std::string(e.what()));
        return 0.0f;
    }
}

std::vector<Polygon> GeometryUtils::bufferPolygon(const Polygon& polygon, float distance) {
    try {
        bg::model::multi_polygon<Polygon> result;
        bg::buffer(polygon, result, 
                  bg::strategy::buffer::distance_symmetric<double>(static_cast<double>(distance)),
                  bg::strategy::buffer::side_straight(),
                  bg::strategy::buffer::join_round(),
                  bg::strategy::buffer::end_round(),
                  bg::strategy::buffer::point_circle());
        
        std::vector<Polygon> polygons;
        for (const auto& poly : result) {
            polygons.push_back(poly);
        }
        return polygons;
    } catch (const std::exception& e) {
        Logger::error("Error in bufferPolygon: " + std::string(e.what()));
        return {polygon}; // 返回原始多边形
    }
}

// 创建矩形多边形的辅助函数
Polygon GeometryUtils::createRectangle(float x, float y, float width, float height) {
    Polygon rect;
    bg::append(rect.outer(), Point2D(x, y));
    bg::append(rect.outer(), Point2D(x + width, y));
    bg::append(rect.outer(), Point2D(x + width, y + height));
    bg::append(rect.outer(), Point2D(x, y + height));
    bg::append(rect.outer(), Point2D(x, y)); // 闭合多边形
    bg::correct(rect);
    return rect;
}

// 创建圆形多边形的辅助函数
Polygon GeometryUtils::createCircle(const Point2D& center, float radius, int segments) {
    Polygon circle;
    const double pi = 3.14159265358979323846;
    
    for (int i = 0; i < segments; ++i) {
        double angle = 2.0 * pi * i / segments;
        double x = center.x() + radius * std::cos(angle);
        double y = center.y() + radius * std::sin(angle);
        bg::append(circle.outer(), Point2D(static_cast<float>(x), static_cast<float>(y)));
    }
    
    bg::correct(circle);
    return circle;
}

// 计算多边形的边界框
Box GeometryUtils::getBoundingBox(const Polygon& polygon) {
    Box bbox;
    bg::envelope(polygon, bbox);
    return bbox;
}

// 检查点是否在边界框内
bool GeometryUtils::pointInBox(const Point2D& point, const Box& box) {
    return bg::within(point, box);
}

} // namespace oneday::pathfinding
