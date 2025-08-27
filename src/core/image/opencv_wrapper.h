#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace oneday::image {

/**
 * @brief OpenCV功能封装类
 * 
 * 提供常用OpenCV功能的简化接口
 */
class OpenCVWrapper {
public:
    /**
     * @brief 构造函数
     */
    OpenCVWrapper();
    
    /**
     * @brief 析构函数
     */
    ~OpenCVWrapper();
    
    // === 图像I/O操作 ===
    
    /**
     * @brief 加载图像
     * @param filename 文件名
     * @param flags 加载标志
     * @return 加载的图像
     */
    cv::Mat loadImage(const std::string& filename, int flags = cv::IMREAD_COLOR);
    
    /**
     * @brief 保存图像
     * @param image 要保存的图像
     * @param filename 文件名
     * @return 是否保存成功
     */
    bool saveImage(const cv::Mat& image, const std::string& filename);
    
    // === 基础图像处理 ===
    
    /**
     * @brief 调整图像尺寸
     * @param src 源图像
     * @param dsize 目标尺寸
     * @param interpolation 插值方法
     * @return 调整后的图像
     */
    cv::Mat resize(const cv::Mat& src, const cv::Size& dsize, int interpolation = cv::INTER_LINEAR);
    
    /**
     * @brief 颜色空间转换
     * @param src 源图像
     * @param code 转换代码
     * @return 转换后的图像
     */
    cv::Mat convertColor(const cv::Mat& src, int code);
    
    // === 滤波操作 ===
    
    /**
     * @brief 高斯模糊
     * @param src 源图像
     * @param ksize 核大小
     * @param sigmaX X方向标准差
     * @param sigmaY Y方向标准差
     * @return 模糊后的图像
     */
    cv::Mat gaussianBlur(const cv::Mat& src, const cv::Size& ksize, double sigmaX, double sigmaY = 0);
    
    /**
     * @brief 双边滤波
     * @param src 源图像
     * @param d 像素邻域直径
     * @param sigmaColor 颜色空间标准差
     * @param sigmaSpace 坐标空间标准差
     * @return 滤波后的图像
     */
    cv::Mat bilateralFilter(const cv::Mat& src, int d, double sigmaColor, double sigmaSpace);
    
    /**
     * @brief 中值滤波
     * @param src 源图像
     * @param ksize 核大小
     * @return 滤波后的图像
     */
    cv::Mat medianBlur(const cv::Mat& src, int ksize);
    
    // === 边缘检测 ===
    
    /**
     * @brief Canny边缘检测
     * @param src 源图像
     * @param threshold1 第一个阈值
     * @param threshold2 第二个阈值
     * @param apertureSize Sobel算子大小
     * @return 边缘图像
     */
    cv::Mat cannyEdgeDetection(const cv::Mat& src, double threshold1, double threshold2, int apertureSize = 3);
    
    // === 特征检测 ===
    
    /**
     * @brief 霍夫圆检测
     * @param src 源图像
     * @param method 检测方法
     * @param dp 累加器分辨率
     * @param minDist 圆心最小距离
     * @param param1 第一个参数
     * @param param2 第二个参数
     * @param minRadius 最小半径
     * @param maxRadius 最大半径
     * @return 检测到的圆
     */
    std::vector<cv::Vec3f> houghCircles(const cv::Mat& src, int method = cv::HOUGH_GRADIENT, 
                                       double dp = 1, double minDist = 50,
                                       double param1 = 200, double param2 = 100, 
                                       int minRadius = 0, int maxRadius = 0);
    
    /**
     * @brief 霍夫直线检测
     * @param src 源图像
     * @param rho 距离分辨率
     * @param theta 角度分辨率
     * @param threshold 阈值
     * @param srn 距离分辨率除数
     * @param stn 角度分辨率除数
     * @param min_theta 最小角度
     * @param max_theta 最大角度
     * @return 检测到的直线
     */
    std::vector<cv::Vec4i> houghLines(const cv::Mat& src, double rho = 1, double theta = CV_PI/180, 
                                     int threshold = 100, double srn = 0, double stn = 0,
                                     double min_theta = 0, double max_theta = CV_PI);
    
    // === 轮廓检测 ===
    
    /**
     * @brief 查找轮廓
     * @param src 源图像
     * @param mode 轮廓检索模式
     * @param method 轮廓近似方法
     * @return 检测到的轮廓
     */
    std::vector<std::vector<cv::Point>> findContours(const cv::Mat& src, 
                                                    int mode = cv::RETR_EXTERNAL, 
                                                    int method = cv::CHAIN_APPROX_SIMPLE);
    
    // === 形态学操作 ===
    
    /**
     * @brief 形态学操作
     * @param src 源图像
     * @param op 操作类型
     * @param kernel 结构元素
     * @param iterations 迭代次数
     * @return 处理后的图像
     */
    cv::Mat morphologyEx(const cv::Mat& src, int op, const cv::Mat& kernel, int iterations = 1);
    
    /**
     * @brief 获取结构元素
     * @param shape 形状
     * @param ksize 大小
     * @param anchor 锚点
     * @return 结构元素
     */
    cv::Mat getStructuringElement(int shape, const cv::Size& ksize, const cv::Point& anchor = cv::Point(-1, -1));
    
    // === 阈值处理 ===
    
    /**
     * @brief 阈值处理
     * @param src 源图像
     * @param thresh 阈值
     * @param maxval 最大值
     * @param type 阈值类型
     * @return 处理后的图像
     */
    cv::Mat threshold(const cv::Mat& src, double thresh, double maxval, int type);
    
    /**
     * @brief 自适应阈值处理
     * @param src 源图像
     * @param maxValue 最大值
     * @param adaptiveMethod 自适应方法
     * @param thresholdType 阈值类型
     * @param blockSize 块大小
     * @param C 常数
     * @return 处理后的图像
     */
    cv::Mat adaptiveThreshold(const cv::Mat& src, double maxValue, int adaptiveMethod, 
                             int thresholdType, int blockSize, double C);
    
    // === 直方图处理 ===
    
    /**
     * @brief 直方图均衡化
     * @param src 源图像
     * @return 均衡化后的图像
     */
    cv::Mat equalizeHist(const cv::Mat& src);
    
    // === 几何计算 ===
    
    /**
     * @brief 计算边界矩形
     * @param points 点集
     * @return 边界矩形
     */
    cv::Rect boundingRect(const std::vector<cv::Point>& points);
    
    /**
     * @brief 计算轮廓面积
     * @param contour 轮廓
     * @return 面积
     */
    double contourArea(const std::vector<cv::Point>& contour);
    
    /**
     * @brief 计算弧长
     * @param curve 曲线
     * @param closed 是否闭合
     * @return 弧长
     */
    double arcLength(const std::vector<cv::Point>& curve, bool closed);
    
    /**
     * @brief 多边形近似
     * @param curve 曲线
     * @param epsilon 精度参数
     * @param closed 是否闭合
     * @return 近似多边形
     */
    std::vector<cv::Point> approxPolyDP(const std::vector<cv::Point>& curve, double epsilon, bool closed);
    
    // === 工具函数 ===
    
    /**
     * @brief 获取OpenCV版本
     * @return 版本字符串
     */
    std::string getOpenCVVersion();
    
    /**
     * @brief 获取图像尺寸
     * @param image 图像
     * @return 尺寸
     */
    cv::Size getImageSize(const cv::Mat& image);
    
    /**
     * @brief 获取图像通道数
     * @param image 图像
     * @return 通道数
     */
    int getImageChannels(const cv::Mat& image);
    
    /**
     * @brief 获取图像深度
     * @param image 图像
     * @return 深度
     */
    int getImageDepth(const cv::Mat& image);
};

} // namespace oneday::image
