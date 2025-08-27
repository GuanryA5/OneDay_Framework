#include <gtest/gtest.h>
#include "core/blueprint/engine.h"
#include "core/blueprint/graph.h"

using namespace oneday::core::blueprint;

class EngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<Engine>();
    }
    
    void TearDown() override {
        engine.reset();
    }
    
    std::unique_ptr<Engine> engine;
};

TEST_F(EngineTest, Initialize) {
    ASSERT_NE(engine, nullptr);
}

TEST_F(EngineTest, ValidateEmptyGraph) {
    BlueprintGraph graph;
    EXPECT_TRUE(engine->validateGraph(graph));
}

TEST_F(EngineTest, ExecuteEmptyGraph) {
    BlueprintGraph graph;
    auto result = engine->executeGraph(graph);
    EXPECT_TRUE(result.success);
}

TEST_F(EngineTest, PauseAndResume) {
    EXPECT_NO_THROW(engine->pauseExecution());
    EXPECT_NO_THROW(engine->resumeExecution());
}