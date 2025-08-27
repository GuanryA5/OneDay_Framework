#include <gtest/gtest.h>
#include "core/common/parallel_utils.h"
#include <vector>
#include <numeric>
#include <chrono>

using namespace oneday::common;

class ParallelUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 重置计数器
        ParallelUtils::reset_task_counter();
    }
    
    void TearDown() override {
        // 清理
        ParallelUtils::reset_task_counter();
    }
};

// 测试推荐线程数获取
TEST_F(ParallelUtilsTest, GetRecommendedThreadCountTest) {
    unsigned int threadCount = ParallelUtils::getRecommendedThreadCount();
    unsigned int cores = std::thread::hardware_concurrency();
    
    EXPECT_GT(threadCount, 0);
    EXPECT_LE(threadCount, cores);
    
    // 如果有多个核心，推荐线程数应该比核心数少1
    if (cores > 2) {
        EXPECT_EQ(threadCount, cores - 1);
    } else {
        EXPECT_EQ(threadCount, 1);
    }
}

// 测试并行for循环
TEST_F(ParallelUtilsTest, ParallelForTest) {
    const size_t size = 1000;
    std::vector<int> data(size, 0);
    
    // 并行设置值
    ParallelUtils::parallel_for(0, size, [&data](size_t i) {
        data[i] = static_cast<int>(i * 2);
    });
    
    // 验证结果
    for (size_t i = 0; i < size; ++i) {
        EXPECT_EQ(data[i], static_cast<int>(i * 2));
    }
}

// 测试空范围的并行for循环
TEST_F(ParallelUtilsTest, ParallelForEmptyRangeTest) {
    std::vector<int> data(10, 0);
    
    // 空范围不应该执行任何操作
    ParallelUtils::parallel_for(5, 5, [&data](size_t i) {
        data[i] = 999; // 不应该被执行
    });
    
    // 验证数据未被修改
    for (int value : data) {
        EXPECT_EQ(value, 0);
    }
}

// 测试并行for_each
TEST_F(ParallelUtilsTest, ParallelForEachTest) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> expected = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};
    
    // 并行处理每个元素
    ParallelUtils::parallel_for_each(data, [](int& value) {
        value *= 2;
    });
    
    // 验证结果
    EXPECT_EQ(data, expected);
}

// 测试并行transform
TEST_F(ParallelUtilsTest, ParallelTransformTest) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    std::vector<int> output;
    
    // 并行变换
    ParallelUtils::parallel_transform(input, output, [](int x) {
        return x * x;
    });
    
    // 验证结果
    std::vector<int> expected = {1, 4, 9, 16, 25};
    EXPECT_EQ(output, expected);
}

// 测试并行reduce
TEST_F(ParallelUtilsTest, ParallelReduceTest) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // 并行求和
    int sum = ParallelUtils::parallel_reduce(data, 0, [](int a, int b) {
        return a + b;
    });
    
    // 验证结果
    int expected = std::accumulate(data.begin(), data.end(), 0);
    EXPECT_EQ(sum, expected);
}

// 测试空容器的并行reduce
TEST_F(ParallelUtilsTest, ParallelReduceEmptyTest) {
    std::vector<int> empty_data;
    
    int result = ParallelUtils::parallel_reduce(empty_data, 42, [](int a, int b) {
        return a + b;
    });
    
    // 空容器应该返回初始值
    EXPECT_EQ(result, 42);
}

// 测试任务执行
TEST_F(ParallelUtilsTest, ExecuteTasksTest) {
    std::vector<int> results(5, 0);
    std::vector<std::function<void()>> tasks;
    
    // 创建任务
    for (int i = 0; i < 5; ++i) {
        tasks.emplace_back([&results, i]() {
            results[i] = i * i;
        });
    }
    
    // 执行任务
    ParallelUtils::execute_tasks(tasks);
    
    // 验证结果
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(results[i], i * i);
    }
}

// 测试空任务列表
TEST_F(ParallelUtilsTest, ExecuteEmptyTasksTest) {
    std::vector<std::function<void()>> empty_tasks;
    
    // 执行空任务列表不应该崩溃
    EXPECT_NO_THROW(ParallelUtils::execute_tasks(empty_tasks));
}

// 测试任务计数器
TEST_F(ParallelUtilsTest, TaskCounterTest) {
    // 初始值应该为0
    EXPECT_EQ(ParallelUtils::get_task_counter(), 0);
    
    // 增加计数器
    ParallelUtils::increment_task_counter();
    EXPECT_EQ(ParallelUtils::get_task_counter(), 1);
    
    // 添加多个
    ParallelUtils::add_task_counter(5);
    EXPECT_EQ(ParallelUtils::get_task_counter(), 6);
    
    // 重置
    ParallelUtils::reset_task_counter();
    EXPECT_EQ(ParallelUtils::get_task_counter(), 0);
}

// 性能测试（可选，用于验证并行性能）
TEST_F(ParallelUtilsTest, PerformanceTest) {
    const size_t large_size = 100000;
    std::vector<int> data(large_size);
    
    // 初始化数据
    std::iota(data.begin(), data.end(), 0);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 并行处理大量数据
    ParallelUtils::parallel_for(0, large_size, [&data](size_t i) {
        data[i] = data[i] * 2 + 1;
    });
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 验证结果正确性
    for (size_t i = 0; i < large_size; ++i) {
        EXPECT_EQ(data[i], static_cast<int>(i * 2 + 1));
    }
    
    // 输出性能信息（仅用于调试）
    std::cout << "并行处理 " << large_size << " 个元素耗时: " 
              << duration.count() << " ms" << std::endl;
}
