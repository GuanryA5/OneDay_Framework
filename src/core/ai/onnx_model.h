#pragma once

#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace oneday::ai {

/**
 * @brief ONNX模型处理类
 *
 * 提供ONNX模型的加载、验证和基础操作功能
 * 为后续集成ONNX Runtime做准备
 */
class ONNXModel {
  public:
    struct ModelInfo {
        std::string name;
        std::string version;
        std::string producer;
        std::vector<std::string> inputNames;
        std::vector<std::string> outputNames;
        std::vector<std::vector<int64_t>> inputShapes;
        std::vector<std::vector<int64_t>> outputShapes;
    };

    /**
     * @brief 构造函数
     */
    ONNXModel() = default;

    /**
     * @brief 析构函数
     */
    ~ONNXModel() = default;

    /**
     * @brief 加载ONNX模型文件
     * @param modelPath 模型文件路径
     * @return 是否加载成功
     */
    bool loadModel(const std::string& modelPath);

    /**
     * @brief 验证模型是否有效
     * @return 是否有效
     */
    bool validateModel() const;

    /**
     * @brief 获取模型信息
     * @return 模型信息结构体
     */
    ModelInfo getModelInfo() const;

    /**
     * @brief 检查模型是否适用于图像处理
     * @return 是否适用于图像处理
     */
    bool isImageProcessingModel() const;

    /**
     * @brief 检查模型是否适用于目标检测
     * @return 是否适用于目标检测
     */
    bool isObjectDetectionModel() const;

    /**
     * @brief 获取推荐的输入图像尺寸
     * @return 推荐的图像尺寸
     */
    cv::Size getRecommendedInputSize() const;

    /**
     * @brief 预处理图像以匹配模型输入要求
     * @param image 输入图像
     * @return 预处理后的图像
     */
    cv::Mat preprocessImage(const cv::Mat& image) const;

    /**
     * @brief 获取模型输入张量的形状
     * @param inputIndex 输入索引
     * @return 张量形状
     */
    std::vector<int64_t> getInputShape(size_t inputIndex = 0) const;

    /**
     * @brief 获取模型输出张量的形状
     * @param outputIndex 输出索引
     * @return 张量形状
     */
    std::vector<int64_t> getOutputShape(size_t outputIndex = 0) const;

    /**
     * @brief 检查模型是否已加载
     * @return 是否已加载
     */
    bool isLoaded() const {
        return m_isLoaded;
    }

    /**
     * @brief 获取模型文件路径
     * @return 模型文件路径
     */
    const std::string& getModelPath() const {
        return m_modelPath;
    }

    /**
     * @brief 打印模型详细信息
     */
    void printModelInfo() const;

    /**
     * @brief 导出模型信息到JSON
     * @return JSON格式的模型信息
     */
    std::string exportModelInfoToJson() const;

  private:
    onnx::ModelProto m_model;
    std::string m_modelPath;
    bool m_isLoaded = false;

    /**
     * @brief 解析输入输出信息
     */
    void parseInputOutputInfo();

    /**
     * @brief 推断模型类型
     */
    void inferModelType();

    /**
     * @brief 验证张量形状
     * @param shape 张量形状
     * @return 是否有效
     */
    bool isValidShape(const std::vector<int64_t>& shape) const;

    // 缓存的模型信息
    mutable ModelInfo m_cachedInfo;
    mutable bool m_infoCached = false;

    // 模型类型标识
    bool m_isImageModel = false;
    bool m_isDetectionModel = false;
    cv::Size m_recommendedInputSize = cv::Size(640, 640);
};

/**
 * @brief ONNX模型管理器
 *
 * 管理多个ONNX模型的加载和使用
 */
class ONNXModelManager {
  public:
    /**
     * @brief 注册模型
     * @param name 模型名称
     * @param modelPath 模型文件路径
     * @return 是否注册成功
     */
    bool registerModel(const std::string& name, const std::string& modelPath);

    /**
     * @brief 获取模型
     * @param name 模型名称
     * @return 模型指针，如果不存在返回nullptr
     */
    std::shared_ptr<ONNXModel> getModel(const std::string& name);

    /**
     * @brief 卸载模型
     * @param name 模型名称
     */
    void unloadModel(const std::string& name);

    /**
     * @brief 获取所有已注册的模型名称
     * @return 模型名称列表
     */
    std::vector<std::string> getModelNames() const;

    /**
     * @brief 清空所有模型
     */
    void clear();

    /**
     * @brief 获取单例实例
     * @return 管理器实例
     */
    static ONNXModelManager& getInstance();

  private:
    std::map<std::string, std::shared_ptr<ONNXModel>> m_models;

    ONNXModelManager() = default;
    ~ONNXModelManager() = default;
    ONNXModelManager(const ONNXModelManager&) = delete;
    ONNXModelManager& operator=(const ONNXModelManager&) = delete;
};

}  // namespace oneday::ai
