#include "onnx_model.h"
#include "../common/logger.h"

using oneday::core::Logger;
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

namespace oneday::ai {

bool ONNXModel::loadModel(const std::string& modelPath) {
    try {
        std::ifstream file(modelPath, std::ios::binary);
        if (!file.is_open()) {
            Logger::error("无法打开ONNX模型文件: " + modelPath);
            return false;
        }
        
        // 读取模型文件
        std::string modelData((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        file.close();
        
        // 解析ONNX模型
        if (!m_model.ParseFromString(modelData)) {
            Logger::error("解析ONNX模型失败: " + modelPath);
            return false;
        }
        
        m_modelPath = modelPath;
        m_isLoaded = true;
        m_infoCached = false;
        
        // 解析模型信息
        parseInputOutputInfo();
        inferModelType();
        
        Logger::info("成功加载ONNX模型: " + modelPath);
        return true;
        
    } catch (const std::exception& e) {
        Logger::error("加载ONNX模型异常: " + std::string(e.what()));
        return false;
    }
}

bool ONNXModel::validateModel() const {
    if (!m_isLoaded) {
        return false;
    }
    
    try {
        // 检查模型基本结�?        if (!m_model.has_graph()) {
            Logger::error("ONNX模型缺少计算�?);
            return false;
        }
        
        const auto& graph = m_model.graph();
        
        // 检查输入输�?        if (graph.input_size() == 0) {
            Logger::error("ONNX模型没有输入");
            return false;
        }
        
        if (graph.output_size() == 0) {
            Logger::error("ONNX模型没有输出");
            return false;
        }
        
        // 检查节�?        if (graph.node_size() == 0) {
            Logger::error("ONNX模型没有计算节点");
            return false;
        }
        
        Logger::info("ONNX模型验证通过");
        return true;
        
    } catch (const std::exception& e) {
        Logger::error("验证ONNX模型异常: " + std::string(e.what()));
        return false;
    }
}

ONNXModel::ModelInfo ONNXModel::getModelInfo() const {
    if (!m_isLoaded) {
        return ModelInfo{};
    }
    
    if (m_infoCached) {
        return m_cachedInfo;
    }
    
    ModelInfo info;
    
    // 基本信息
    if (m_model.has_model_version()) {
        info.version = std::to_string(m_model.model_version());
    }
    
    if (m_model.has_producer_name()) {
        info.producer = m_model.producer_name();
    }
    
    if (m_model.has_graph()) {
        const auto& graph = m_model.graph();
        info.name = graph.name();
        
        // 输入信息
        for (const auto& input : graph.input()) {
            info.inputNames.push_back(input.name());
            
            std::vector<int64_t> shape;
            if (input.has_type() && input.type().has_tensor_type()) {
                const auto& tensorType = input.type().tensor_type();
                if (tensorType.has_shape()) {
                    for (const auto& dim : tensorType.shape().dim()) {
                        if (dim.has_dim_value()) {
                            shape.push_back(dim.dim_value());
                        } else {
                            shape.push_back(-1); // 动态维�?                        }
                    }
                }
            }
            info.inputShapes.push_back(shape);
        }
        
        // 输出信息
        for (const auto& output : graph.output()) {
            info.outputNames.push_back(output.name());
            
            std::vector<int64_t> shape;
            if (output.has_type() && output.type().has_tensor_type()) {
                const auto& tensorType = output.type().tensor_type();
                if (tensorType.has_shape()) {
                    for (const auto& dim : tensorType.shape().dim()) {
                        if (dim.has_dim_value()) {
                            shape.push_back(dim.dim_value());
                        } else {
                            shape.push_back(-1); // 动态维�?                        }
                    }
                }
            }
            info.outputShapes.push_back(shape);
        }
    }
    
    m_cachedInfo = info;
    m_infoCached = true;
    
    return info;
}

bool ONNXModel::isImageProcessingModel() const {
    return m_isImageModel;
}

bool ONNXModel::isObjectDetectionModel() const {
    return m_isDetectionModel;
}

cv::Size ONNXModel::getRecommendedInputSize() const {
    return m_recommendedInputSize;
}

cv::Mat ONNXModel::preprocessImage(const cv::Mat& image) const {
    if (!m_isLoaded || !m_isImageModel) {
        return image;
    }
    
    cv::Mat processed;
    
    // 调整尺寸
    cv::resize(image, processed, m_recommendedInputSize);
    
    // 转换颜色空间 (BGR -> RGB)
    cv::cvtColor(processed, processed, cv::COLOR_BGR2RGB);
    
    // 归一化到 [0, 1]
    processed.convertTo(processed, CV_32F, 1.0 / 255.0);
    
    return processed;
}

std::vector<int64_t> ONNXModel::getInputShape(size_t inputIndex) const {
    auto info = getModelInfo();
    if (inputIndex < info.inputShapes.size()) {
        return info.inputShapes[inputIndex];
    }
    return {};
}

std::vector<int64_t> ONNXModel::getOutputShape(size_t outputIndex) const {
    auto info = getModelInfo();
    if (outputIndex < info.outputShapes.size()) {
        return info.outputShapes[outputIndex];
    }
    return {};
}

void ONNXModel::printModelInfo() const {
    auto info = getModelInfo();
    
    Logger::info("=== ONNX模型信息 ===");
    Logger::info("名称: " + info.name);
    Logger::info("版本: " + info.version);
    Logger::info("生产�? " + info.producer);
    Logger::info("文件路径: " + m_modelPath);
    
    Logger::info("输入信息:");
    for (size_t i = 0; i < info.inputNames.size(); ++i) {
        std::string shapeStr = "[";
        for (size_t j = 0; j < info.inputShapes[i].size(); ++j) {
            if (j > 0) shapeStr += ", ";
            shapeStr += std::to_string(info.inputShapes[i][j]);
        }
        shapeStr += "]";
        Logger::info("  " + info.inputNames[i] + ": " + shapeStr);
    }
    
    Logger::info("输出信息:");
    for (size_t i = 0; i < info.outputNames.size(); ++i) {
        std::string shapeStr = "[";
        for (size_t j = 0; j < info.outputShapes[i].size(); ++j) {
            if (j > 0) shapeStr += ", ";
            shapeStr += std::to_string(info.outputShapes[i][j]);
        }
        shapeStr += "]";
        Logger::info("  " + info.outputNames[i] + ": " + shapeStr);
    }
    
    Logger::info("模型类型:");
    Logger::info("  图像处理模型: " + std::string(m_isImageModel ? "�? : "�?));
    Logger::info("  目标检测模�? " + std::string(m_isDetectionModel ? "�? : "�?));
    Logger::info("  推荐输入尺寸: " + std::to_string(m_recommendedInputSize.width) + 
                "x" + std::to_string(m_recommendedInputSize.height));
    Logger::info("==================");
}

std::string ONNXModel::exportModelInfoToJson() const {
    auto info = getModelInfo();
    nlohmann::json j;
    
    j["name"] = info.name;
    j["version"] = info.version;
    j["producer"] = info.producer;
    j["modelPath"] = m_modelPath;
    j["isImageModel"] = m_isImageModel;
    j["isDetectionModel"] = m_isDetectionModel;
    j["recommendedInputSize"] = {
        {"width", m_recommendedInputSize.width},
        {"height", m_recommendedInputSize.height}
    };
    
    j["inputs"] = nlohmann::json::array();
    for (size_t i = 0; i < info.inputNames.size(); ++i) {
        nlohmann::json input;
        input["name"] = info.inputNames[i];
        input["shape"] = info.inputShapes[i];
        j["inputs"].push_back(input);
    }
    
    j["outputs"] = nlohmann::json::array();
    for (size_t i = 0; i < info.outputNames.size(); ++i) {
        nlohmann::json output;
        output["name"] = info.outputNames[i];
        output["shape"] = info.outputShapes[i];
        j["outputs"].push_back(output);
    }
    
    return j.dump(2);
}

void ONNXModel::parseInputOutputInfo() {
    // 这个方法在getModelInfo中已经实现了解析逻辑
    // 这里可以添加额外的解析逻辑
}

void ONNXModel::inferModelType() {
    if (!m_isLoaded) return;
    
    auto info = getModelInfo();
    
    // 推断是否为图像处理模�?    if (!info.inputShapes.empty()) {
        const auto& firstInputShape = info.inputShapes[0];
        
        // 检查是否有4维输�?(batch, channel, height, width)
        if (firstInputShape.size() == 4) {
            m_isImageModel = true;
            
            // 获取推荐输入尺寸
            if (firstInputShape[2] > 0 && firstInputShape[3] > 0) {
                m_recommendedInputSize = cv::Size(
                    static_cast<int>(firstInputShape[3]), 
                    static_cast<int>(firstInputShape[2])
                );
            }
        }
    }
    
    // 推断是否为目标检测模�?    if (m_isImageModel && !info.outputShapes.empty()) {
        // 检查输出形状是否符合检测模型特�?        for (const auto& outputShape : info.outputShapes) {
            // 常见的检测输出格�? [batch, num_detections, 6] 或类�?            if (outputShape.size() >= 2 && outputShape.back() >= 4) {
                m_isDetectionModel = true;
                break;
            }
        }
    }
}

bool ONNXModel::isValidShape(const std::vector<int64_t>& shape) const {
    return !shape.empty() && 
           std::all_of(shape.begin(), shape.end(), 
                      [](int64_t dim) { return dim > 0 || dim == -1; });
}

// ONNXModelManager 实现
bool ONNXModelManager::registerModel(const std::string& name, const std::string& modelPath) {
    auto model = std::make_shared<ONNXModel>();
    if (model->loadModel(modelPath)) {
        m_models[name] = model;
        Logger::info("注册ONNX模型: " + name + " -> " + modelPath);
        return true;
    }
    return false;
}

std::shared_ptr<ONNXModel> ONNXModelManager::getModel(const std::string& name) {
    auto it = m_models.find(name);
    return (it != m_models.end()) ? it->second : nullptr;
}

void ONNXModelManager::unloadModel(const std::string& name) {
    m_models.erase(name);
    Logger::info("卸载ONNX模型: " + name);
}

std::vector<std::string> ONNXModelManager::getModelNames() const {
    std::vector<std::string> names;
    for (const auto& pair : m_models) {
        names.push_back(pair.first);
    }
    return names;
}

void ONNXModelManager::clear() {
    m_models.clear();
    Logger::info("清空所有ONNX模型");
}

ONNXModelManager& ONNXModelManager::getInstance() {
    static ONNXModelManager instance;
    return instance;
}

} // namespace oneday::ai
