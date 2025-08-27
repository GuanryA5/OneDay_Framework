#include <gtest/gtest.h>
#include "core/blueprint/engine.h"
#include "core/blueprint/graph.h"

using namespace oneday::core::blueprint;

class BlueprintIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<Engine>();
        graph = std::make_unique<BlueprintGraph>();
    }
    
    void TearDown() override {
        engine.reset();
        graph.reset();
    }
    
    std::unique_ptr<Engine> engine;
    std::unique_ptr<BlueprintGraph> graph;
};

TEST_F(BlueprintIntegrationTest, EngineGraphInteraction) {
    // 设置图表
    graph->setName("Integration Test Graph");
    
    // 验证图表
    EXPECT_TRUE(engine->validateGraph(*graph));
    
    // 执行图表
    auto result = engine->executeGraph(*graph);
    EXPECT_TRUE(result.success);
}

TEST_F(BlueprintIntegrationTest, LoadGraphAndExecute) {
    std::string testJson = R"({
        "id": "integration_test",
        "name": "Integration Test Graph",
        "nodes": []
    })";
    
    EXPECT_TRUE(graph->loadFromJson(testJson));
    EXPECT_TRUE(engine->validateGraph(*graph));
    
    auto result = engine->executeGraph(*graph);
    EXPECT_TRUE(result.success);
}