#include "core/ai/onnx_model.h"

#include <gtest/gtest.h>
#include <onnx/onnx.pb.h>

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace oneday::ai;

class ONNXModelTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // 创建测试目录
        testDir = std::filesystem::temp_directory_path() / "onnx_test";
        std::filesystem::create_directories(testDir);

        // 创建一个简单的测试ONNX模型文件（模拟）
        createMockONNXModel();
    }

    void TearDown() override {
        // 清理测试文件
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }

    void createMockONNXModel() {
        // 创建一个简单的ONNX模型原型
        // 注意：这里创建的是一个最小化的ONNX模型结构
        onnx::ModelProto model;
        model.set_ir_version(7);
        model.set_model_version(1);
        model.set_producer_name("test_producer");

        // 创建计算图
        auto* graph = model.mutable_graph();
        graph->set_name("test_graph");

        // 添加输入
        auto* input = graph->add_input();
        input->set_name("input");
        auto* inputType = input->mutable_type();
        auto* tensorType = inputType->mutable_tensor_type();
        tensorType->set_elem_type(onnx::TensorProto::FLOAT);
        auto* shape = tensorType->mutable_shape();

        // 添加维度 [1, 3, 224, 224] - 典型的图像输入
        auto* dim1 = shape->add_dim();
        dim1->set_dim_value(1);
        auto* dim2 = shape->add_dim();
        dim2->set_dim_value(3);
        auto* dim3 = shape->add_dim();
        dim3->set_dim_value(224);
        auto* dim4 = shape->add_dim();
        dim4->set_dim_value(224);

        // 添加输出
        auto* output = graph->add_output();
        output->set_name("output");
        auto* outputType = output->mutable_type();
        auto* outputTensorType = outputType->mutable_tensor_type();
        outputTensorType->set_elem_type(onnx::TensorProto::FLOAT);
        auto* outputShape = outputTensorType->mutable_shape();
        auto* outputDim1 = outputShape->add_dim();
        outputDim1->set_dim_value(1);
        auto* outputDim2 = outputShape->add_dim();
        outputDim2->set_dim_value(1000);  // 分类输出

        // 添加一个简单的节点
        auto* node = graph->add_node();
        node->set_op_type("Identity");
        node->add_input("input");
        node->add_output("output");

        // 序列化并保存
        std::string serialized;
        model.SerializeToString(&serialized);

        mockModelPath = testDir / "test_model.onnx";
        std::ofstream file(mockModelPath, std::ios::binary);
        file.write(serialized.data(), serialized.size());
        file.close();
    }

    std::filesystem::path testDir;
    std::filesystem::path mockModelPath;
};

// 测试模型加载
TEST_F(ONNXModelTest, LoadModelTest) {
    ONNXModel model;

    // 测试加载存在的模型
    EXPECT_TRUE(model.loadModel(mockModelPath.string()));
    EXPECT_TRUE(model.isLoaded());
    EXPECT_EQ(model.getModelPath(), mockModelPath.string());

    // 测试加载不存在的模型
    ONNXModel model2;
    EXPECT_FALSE(model2.loadModel("nonexistent_model.onnx"));
    EXPECT_FALSE(model2.isLoaded());
}

// 测试模型验证
TEST_F(ONNXModelTest, ValidateModelTest) {
    ONNXModel model;

    // 未加载的模型应该验证失败
    EXPECT_FALSE(model.validateModel());

    // 加载模型后应该验证成功
    ASSERT_TRUE(model.loadModel(mockModelPath.string()));
    EXPECT_TRUE(model.validateModel());
}

// 测试模型信息获取
TEST_F(ONNXModelTest, GetModelInfoTest) {
    ONNXModel model;
    ASSERT_TRUE(model.loadModel(mockModelPath.string()));

    auto info = model.getModelInfo();

    EXPECT_EQ(info.name, "test_graph");
    EXPECT_EQ(info.version, "1");
    EXPECT_EQ(info.producer, "test_producer");

    // 检查输入信息
    ASSERT_EQ(info.inputNames.size(), 1);
    EXPECT_EQ(info.inputNames[0], "input");
    ASSERT_EQ(info.inputShapes.size(), 1);
    ASSERT_EQ(info.inputShapes[0].size(), 4);
    EXPECT_EQ(info.inputShapes[0][0], 1);
    EXPECT_EQ(info.inputShapes[0][1], 3);
    EXPECT_EQ(info.inputShapes[0][2], 224);
    EXPECT_EQ(info.inputShapes[0][3], 224);

    // 检查输出信息
    ASSERT_EQ(info.outputNames.size(), 1);
    EXPECT_EQ(info.outputNames[0], "output");
    ASSERT_EQ(info.outputShapes.size(), 1);
    ASSERT_EQ(info.outputShapes[0].size(), 2);
    EXPECT_EQ(info.outputShapes[0][0], 1);
    EXPECT_EQ(info.outputShapes[0][1], 1000);
}

// 测试模型类型推断
TEST_F(ONNXModelTest, ModelTypeInferenceTest) {
    ONNXModel model;
    ASSERT_TRUE(model.loadModel(mockModelPath.string()));

    // 应该被识别为图像处理模型
    EXPECT_TRUE(model.isImageProcessingModel());

    // 不应该被识别为目标检测模型（输出形状不符合）
    EXPECT_FALSE(model.isObjectDetectionModel());

    // 检查推荐输入尺寸
    cv::Size recommendedSize = model.getRecommendedInputSize();
    EXPECT_EQ(recommendedSize.width, 224);
    EXPECT_EQ(recommendedSize.height, 224);
}

// 测试图像预处理
TEST_F(ONNXModelTest, PreprocessImageTest) {
    ONNXModel model;
    ASSERT_TRUE(model.loadModel(mockModelPath.string()));

    // 创建测试图像
    cv::Mat testImage = cv::Mat::zeros(480, 640, CV_8UC3);
    testImage.setTo(cv::Scalar(128, 128, 128));  // 灰色图像

    // 预处理图像
    cv::Mat processed = model.preprocessImage(testImage);

    // 检查预处理结果
    EXPECT_EQ(processed.size(), cv::Size(224, 224));
    EXPECT_EQ(processed.type(), CV_32F);

    // 检查像素值是否在正确范围内 [0, 1]
    double minVal, maxVal;
    cv::minMaxLoc(processed, &minVal, &maxVal);
    EXPECT_GE(minVal, 0.0);
    EXPECT_LE(maxVal, 1.0);
}

// 测试输入输出形状获取
TEST_F(ONNXModelTest, GetShapeTest) {
    ONNXModel model;
    ASSERT_TRUE(model.loadModel(mockModelPath.string()));

    // 测试输入形状
    auto inputShape = model.getInputShape(0);
    ASSERT_EQ(inputShape.size(), 4);
    EXPECT_EQ(inputShape[0], 1);
    EXPECT_EQ(inputShape[1], 3);
    EXPECT_EQ(inputShape[2], 224);
    EXPECT_EQ(inputShape[3], 224);

    // 测试输出形状
    auto outputShape = model.getOutputShape(0);
    ASSERT_EQ(outputShape.size(), 2);
    EXPECT_EQ(outputShape[0], 1);
    EXPECT_EQ(outputShape[1], 1000);

    // 测试无效索引
    auto invalidShape = model.getInputShape(999);
    EXPECT_TRUE(invalidShape.empty());
}

// 测试JSON导出
TEST_F(ONNXModelTest, ExportToJsonTest) {
    ONNXModel model;
    ASSERT_TRUE(model.loadModel(mockModelPath.string()));

    std::string jsonStr = model.exportModelInfoToJson();
    EXPECT_FALSE(jsonStr.empty());

    // 验证JSON格式
    nlohmann::json j = nlohmann::json::parse(jsonStr);
    EXPECT_EQ(j["name"], "test_graph");
    EXPECT_EQ(j["version"], "1");
    EXPECT_EQ(j["producer"], "test_producer");
    EXPECT_TRUE(j["isImageModel"]);
    EXPECT_FALSE(j["isDetectionModel"]);
}

// 测试模型管理器
TEST_F(ONNXModelTest, ModelManagerTest) {
    auto& manager = ONNXModelManager::getInstance();

    // 清空管理器
    manager.clear();
    EXPECT_TRUE(manager.getModelNames().empty());

    // 注册模型
    EXPECT_TRUE(manager.registerModel("test_model", mockModelPath.string()));

    // 检查注册结果
    auto names = manager.getModelNames();
    ASSERT_EQ(names.size(), 1);
    EXPECT_EQ(names[0], "test_model");

    // 获取模型
    auto model = manager.getModel("test_model");
    ASSERT_NE(model, nullptr);
    EXPECT_TRUE(model->isLoaded());

    // 获取不存在的模型
    auto nonexistentModel = manager.getModel("nonexistent");
    EXPECT_EQ(nonexistentModel, nullptr);

    // 卸载模型
    manager.unloadModel("test_model");
    EXPECT_TRUE(manager.getModelNames().empty());
}
