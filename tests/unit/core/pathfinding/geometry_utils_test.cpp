#include <gtest/gtest.h>
#include "core/pathfinding/geometry_utils.h"

using namespace oneday::pathfinding;

class GeometryUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建测试用的矩形
        testRect = GeometryUtils::createRectangle(0.0f, 0.0f, 10.0f, 10.0f);
        
        // 创建测试用的圆形
        testCircle = GeometryUtils::createCircle(Point2D(5.0f, 5.0f), 3.0f, 16);
    }
    
    Polygon testRect;
    Polygon testCircle;
};

// 测试点在多边形内检测
TEST_F(GeometryUtilsTest, PointInPolygonTest) {
    // 测试点在矩形内
    EXPECT_TRUE(GeometryUtils::pointInPolygon(Point2D(5.0f, 5.0f), testRect));
    
    // 测试点在矩形外
    EXPECT_FALSE(GeometryUtils::pointInPolygon(Point2D(15.0f, 15.0f), testRect));
    
    // 测试边界点
    EXPECT_TRUE(GeometryUtils::pointInPolygon(Point2D(0.0f, 0.0f), testRect));
}

// 测试距离计算
TEST_F(GeometryUtilsTest, DistanceTest) {
    Point2D p1(0.0f, 0.0f);
    Point2D p2(3.0f, 4.0f);
    
    float distance = GeometryUtils::distance(p1, p2);
    EXPECT_FLOAT_EQ(distance, 5.0f); // 3-4-5直角三角形
}

// 测试多边形相交
TEST_F(GeometryUtilsTest, PolygonsIntersectTest) {
    // 创建一个与testRect相交的矩形
    Polygon intersectingRect = GeometryUtils::createRectangle(5.0f, 5.0f, 10.0f, 10.0f);
    EXPECT_TRUE(GeometryUtils::polygonsIntersect(testRect, intersectingRect));
    
    // 创建一个不相交的矩形
    Polygon nonIntersectingRect = GeometryUtils::createRectangle(20.0f, 20.0f, 5.0f, 5.0f);
    EXPECT_FALSE(GeometryUtils::polygonsIntersect(testRect, nonIntersectingRect));
}

// 测试线段与多边形相交
TEST_F(GeometryUtilsTest, LineIntersectsPolygonTest) {
    // 穿过矩形的线段
    EXPECT_TRUE(GeometryUtils::lineIntersectsPolygon(
        Point2D(-1.0f, 5.0f), Point2D(11.0f, 5.0f), testRect));
    
    // 不穿过矩形的线段
    EXPECT_FALSE(GeometryUtils::lineIntersectsPolygon(
        Point2D(-5.0f, -5.0f), Point2D(-1.0f, -1.0f), testRect));
}

// 测试多边形面积计算
TEST_F(GeometryUtilsTest, PolygonAreaTest) {
    float area = GeometryUtils::polygonArea(testRect);
    EXPECT_FLOAT_EQ(area, 100.0f); // 10x10矩形的面积
}

// 测试多边形简化
TEST_F(GeometryUtilsTest, SimplifyPolygonTest) {
    // 创建一个复杂的多边形
    Polygon complexPoly;
    bg::append(complexPoly.outer(), Point2D(0.0f, 0.0f));
    bg::append(complexPoly.outer(), Point2D(1.0f, 0.1f));
    bg::append(complexPoly.outer(), Point2D(2.0f, 0.0f));
    bg::append(complexPoly.outer(), Point2D(2.0f, 2.0f));
    bg::append(complexPoly.outer(), Point2D(0.0f, 2.0f));
    bg::append(complexPoly.outer(), Point2D(0.0f, 0.0f));
    bg::correct(complexPoly);
    
    Polygon simplified = GeometryUtils::simplifyPolygon(complexPoly, 0.5f);
    
    // 简化后的多边形应该有更少的顶点
    EXPECT_LE(simplified.outer().size(), complexPoly.outer().size());
}

// 测试边界框
TEST_F(GeometryUtilsTest, BoundingBoxTest) {
    Box bbox = GeometryUtils::getBoundingBox(testRect);
    
    EXPECT_FLOAT_EQ(bbox.min_corner().x(), 0.0f);
    EXPECT_FLOAT_EQ(bbox.min_corner().y(), 0.0f);
    EXPECT_FLOAT_EQ(bbox.max_corner().x(), 10.0f);
    EXPECT_FLOAT_EQ(bbox.max_corner().y(), 10.0f);
}

// 测试点在边界框内
TEST_F(GeometryUtilsTest, PointInBoxTest) {
    Box bbox = GeometryUtils::getBoundingBox(testRect);
    
    EXPECT_TRUE(GeometryUtils::pointInBox(Point2D(5.0f, 5.0f), bbox));
    EXPECT_FALSE(GeometryUtils::pointInBox(Point2D(15.0f, 15.0f), bbox));
}

// 测试坐标转换
TEST_F(GeometryUtilsTest, CoordinateConversionTest) {
    Point2D originalPoint(3.5f, 7.2f);
    
    // Point2D -> Eigen -> Point2D
    Vector2f eigenVec = GeometryUtils::toEigen(originalPoint);
    Point2D convertedPoint = GeometryUtils::fromEigen(eigenVec);
    
    EXPECT_FLOAT_EQ(originalPoint.x(), convertedPoint.x());
    EXPECT_FLOAT_EQ(originalPoint.y(), convertedPoint.y());
}

// 测试圆形创建
TEST_F(GeometryUtilsTest, CreateCircleTest) {
    Point2D center(0.0f, 0.0f);
    float radius = 5.0f;
    Polygon circle = GeometryUtils::createCircle(center, radius, 8);
    
    // 检查圆形是否有正确的顶点数 (8个顶点 + 1个闭合点)
    EXPECT_EQ(circle.outer().size(), 9);
    
    // 检查圆心是否在圆内
    EXPECT_TRUE(GeometryUtils::pointInPolygon(center, circle));
}

// 测试矩形创建
TEST_F(GeometryUtilsTest, CreateRectangleTest) {
    Polygon rect = GeometryUtils::createRectangle(1.0f, 2.0f, 3.0f, 4.0f);
    
    // 矩形应该有5个点 (4个角 + 1个闭合点)
    EXPECT_EQ(rect.outer().size(), 5);
    
    // 检查面积
    float area = GeometryUtils::polygonArea(rect);
    EXPECT_FLOAT_EQ(area, 12.0f); // 3x4 = 12
}
