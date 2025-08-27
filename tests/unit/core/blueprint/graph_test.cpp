#include "core/blueprint/graph.h"

#include <gtest/gtest.h>

using namespace oneday::core::blueprint;

class GraphTest : public ::testing::Test {
  protected:
    void SetUp() override {
        graph = std::make_unique<BlueprintGraph>();
    }

    void TearDown() override {
        graph.reset();
    }

    std::unique_ptr<BlueprintGraph> graph;
};

TEST_F(GraphTest, Initialize) {
    ASSERT_NE(graph, nullptr);
    EXPECT_FALSE(graph->getId().empty());
}

TEST_F(GraphTest, SetAndGetName) {
    const std::string testName = "TestGraph";
    graph->setName(testName);
    EXPECT_EQ(graph->getName(), testName);
}

TEST_F(GraphTest, ExportToJson) {
    graph->setName("TestGraph");
    std::string json = graph->exportToJson();
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("TestGraph"), std::string::npos);
}

TEST_F(GraphTest, ImportFromJson) {
    std::string testJson = R"({
        "id": "test_graph_123",
        "name": "Test Graph",
        "nodes": []
    })";

    EXPECT_TRUE(graph->importFromJson(testJson));
    EXPECT_EQ(graph->getName(), "Test Graph");
    EXPECT_EQ(graph->getId(), "test_graph_123");
}