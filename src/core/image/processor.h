#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace oneday::image {

/**
 * @brief 图像处理器类
 * 
 * 提供常用的图像处理功能，包括滤波、边缘检测、对象检测等
 */
class ImageProcessor {
public:
    /**
     * @brief 构造函数
     */
    ImageProcessor();
    
    /**
     * @brief 析构函数
     */
    ~ImageProcessor();
    
    /**
     * @brief 处理图像（基础处理）
     * @param input 输入图像
     * @return 处理后的图像
     */
    cv::Mat processImage(const cv::Mat& input);
    
    /**
     * @brief 调整图像尺寸
     * @param input 输入图像
     * @param size 目标尺寸
     * @return 调整后的图像
     */
    cv::Mat resize(const cv::Mat& input, const cv::Size& size);
    
    /**
     * @brief 转换颜色空间
     * @param input 输入图像
     * @param code 转换代码（如CV_BGR2GRAY）
     * @return 转换后的图像
     */
    cv::Mat convertColorSpace(const cv::Mat& input, int code);
    
    /**
     * @brief 应用高斯模糊
     * @param input 输入图像
     * @param kernelSize 核大小
     * @param sigmaX X方向标准差
     * @param sigmaY Y方向标准差
     * @return 模糊后的图像
     */
    cv::Mat applyGaussianBlur(const cv::Mat& input, const cv::Size& kernelSize, 
                             double sigmaX, double sigmaY = 0);
    
    /**
     * @brief 边缘检测
     * @param input 输入图像
     * @param threshold1 第一个阈值
     * @param threshold2 第二个阈值
     * @return 边缘图像
     */
    cv::Mat detectEdges(const cv::Mat& input, double threshold1 = 100, double threshold2 = 200);
    
    /**
     * @brief 对象检测（使用Haar级联分类器）
     * @param input 输入图像
     * @param cascadePath 级联分类器文件路径
     * @return 检测到的对象矩形列表
     */
    std::vector<cv::Rect> detectObjects(const cv::Mat& input, const std::string& cascadePath);
    
    /**
     * @brief 增强对比度
     * @param input 输入图像
     * @param alpha 对比度系数
     * @param beta 亮度偏移
     * @return 增强后的图像
     */
    cv::Mat enhanceContrast(const cv::Mat& input, double alpha = 1.5, int beta = 0);
    
    /**
     * @brief 直方图均衡化
     * @param input 输入图像
     * @return 均衡化后的图像
     */
    cv::Mat applyHistogramEqualization(const cv::Mat& input);
    
    /**
     * @brief 保存图像
     * @param image 要保存的图像
     * @param filename 文件名
     * @return 是否保存成功
     */
    bool saveImage(const cv::Mat& image, const std::string& filename);
    
    /**
     * @brief 加载图像
     * @param filename 文件名
     * @param flags 加载标志（如IMREAD_COLOR）
     * @return 加载的图像
     */
    cv::Mat loadImage(const std::string& filename, int flags = cv::IMREAD_COLOR);

private:
    // 私有成员变量可以在这里添加
};

} // namespace oneday::image
