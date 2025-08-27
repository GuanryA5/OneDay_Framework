#include <gtest/gtest.h>
#include <chrono>
#include "core/blueprint/engine.h"
#include "core/blueprint/graph.h"

using namespace oneday::core::blueprint;
using namespace std::chrono;

class BlueprintPerformanceTest : public ::testing::Test {
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

TEST_F(BlueprintPerformanceTest, GraphExecutionPerformance) {
    const int iterations = 1000;
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto result = engine->executeGraph(*graph);
        EXPECT_TRUE(result.success);
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    
    // 性能要求：1000次执行应该在1秒内完成
    EXPECT_LT(duration.count(), 1000) 
        << "执行" << iterations << "次用时" << duration.count() << "ms";
}

TEST_F(BlueprintPerformanceTest, GraphValidationPerformance) {
    const int iterations = 10000;
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        EXPECT_TRUE(engine->validateGraph(*graph));
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    
    // 性能要求：10000次验证应该在500ms内完成
    EXPECT_LT(duration.count(), 500)
        << "验证" << iterations << "次用时" << duration.count() << "ms";
}