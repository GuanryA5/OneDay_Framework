#pragma once

#include <algorithm>
#include <execution>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

namespace oneday::common {

/**
 * @brief 并行计算工具类 - 替代TBB功能
 *
 * 使用C++17标准库的并行算法和线程池提供高性能并行计算
 * 专门针对游戏脚本场景优化
 */
class ParallelUtils {
  public:
    /**
     * @brief 获取推荐的线程数量
     * @return 推荐的工作线程数量
     */
    static unsigned int getRecommendedThreadCount() {
        unsigned int cores = std::thread::hardware_concurrency();
        return cores > 2 ? cores - 1 : 1;  // 保留一个核心给系统
    }

    /**
     * @brief 并行for循环 - 替代tbb::parallel_for
     * @param start 起始索引
     * @param end 结束索引
     * @param func 执行函数 void(size_t index)
     */
    template <typename Func>
    static void parallel_for(size_t start, size_t end, Func&& func) {
        if (end <= start)
            return;

        const size_t numThreads = getRecommendedThreadCount();
        const size_t totalWork = end - start;

        if (totalWork < numThreads || numThreads == 1) {
            // 工作量小或单线程，直接执行
            for (size_t i = start; i < end; ++i) {
                func(i);
            }
            return;
        }

        // 多线程并行执行
        std::vector<std::future<void>> futures;
        const size_t workPerThread = totalWork / numThreads;

        for (size_t t = 0; t < numThreads; ++t) {
            size_t threadStart = start + t * workPerThread;
            size_t threadEnd = (t == numThreads - 1) ? end : threadStart + workPerThread;

            futures.emplace_back(std::async(std::launch::async, [threadStart, threadEnd, &func]() {
                for (size_t i = threadStart; i < threadEnd; ++i) {
                    func(i);
                }
            }));
        }

        // 等待所有线程完成
        for (auto& future : futures) {
            future.wait();
        }
    }

    /**
     * @brief 并行处理容器 - 替代tbb::parallel_for_each
     * @param container 容器
     * @param func 处理函数
     */
    template <typename Container, typename Func>
    static void parallel_for_each(Container& container, Func&& func) {
        if constexpr (std::is_same_v<std::execution::parallel_unsequenced_policy,
                                     decltype(std::execution::par_unseq)>) {
            // 使用C++17并行算法（如果支持）
            std::for_each(std::execution::par_unseq, container.begin(), container.end(), func);
        } else {
            // 回退到自定义并行实现
            parallel_for(
                0, container.size(), [&container, &func](size_t i) { func(container[i]); });
        }
    }

    /**
     * @brief 并行变换容器 - 替代tbb::parallel_transform
     * @param input 输入容器
     * @param output 输出容器
     * @param func 变换函数
     */
    template <typename InputContainer, typename OutputContainer, typename Func>
    static void parallel_transform(const InputContainer& input,
                                   OutputContainer& output,
                                   Func&& func) {
        output.resize(input.size());

        if constexpr (std::is_same_v<std::execution::parallel_unsequenced_policy,
                                     decltype(std::execution::par_unseq)>) {
            // 使用C++17并行算法
            std::transform(
                std::execution::par_unseq, input.begin(), input.end(), output.begin(), func);
        } else {
            // 自定义并行实现
            parallel_for(0, input.size(), [&input, &output, &func](size_t i) {
                output[i] = func(input[i]);
            });
        }
    }

    /**
     * @brief 并行归约 - 替代tbb::parallel_reduce
     * @param container 容器
     * @param init 初始值
     * @param reduce_func 归约函数
     * @return 归约结果
     */
    template <typename Container, typename T, typename ReduceFunc>
    static T parallel_reduce(const Container& container, T init, ReduceFunc&& reduce_func) {
        if (container.empty())
            return init;

        const size_t numThreads = getRecommendedThreadCount();
        const size_t totalWork = container.size();

        if (totalWork < numThreads || numThreads == 1) {
            // 单线程归约
            T result = init;
            for (const auto& item : container) {
                result = reduce_func(result, item);
            }
            return result;
        }

        // 多线程并行归约
        std::vector<std::future<T>> futures;
        const size_t workPerThread = totalWork / numThreads;

        for (size_t t = 0; t < numThreads; ++t) {
            size_t threadStart = t * workPerThread;
            size_t threadEnd = (t == numThreads - 1) ? totalWork : threadStart + workPerThread;

            futures.emplace_back(std::async(
                std::launch::async, [&container, threadStart, threadEnd, init, &reduce_func]() {
                    T localResult = init;
                    for (size_t i = threadStart; i < threadEnd; ++i) {
                        localResult = reduce_func(localResult, container[i]);
                    }
                    return localResult;
                }));
        }

        // 合并结果
        T finalResult = init;
        for (auto& future : futures) {
            finalResult = reduce_func(finalResult, future.get());
        }

        return finalResult;
    }

    /**
     * @brief 简单的线程池任务执行
     * @param tasks 任务列表
     */
    template <typename Func>
    static void execute_tasks(std::vector<Func>& tasks) {
        if (tasks.empty())
            return;

        const size_t numThreads =
            std::min(static_cast<size_t>(getRecommendedThreadCount()), tasks.size());

        if (numThreads == 1) {
            // 单线程执行
            for (auto& task : tasks) {
                task();
            }
            return;
        }

        // 多线程执行
        std::vector<std::future<void>> futures;
        size_t taskIndex = 0;

        for (size_t t = 0; t < numThreads; ++t) {
            futures.emplace_back(
                std::async(std::launch::async, [&tasks, &taskIndex, numThreads, t]() {
                    for (size_t i = t; i < tasks.size(); i += numThreads) {
                        tasks[i]();
                    }
                }));
        }

        // 等待所有任务完成
        for (auto& future : futures) {
            future.wait();
        }
    }

    // === 性能监控和调试功能 ===

    /**
     * @brief 执行并行计算性能基准测试
     */
    static void benchmark_parallel_performance();

    /**
     * @brief 输出系统信息到日志
     */
    static void log_system_info();

    /**
     * @brief 增加任务计数器（用于性能监控）
     */
    static void increment_task_counter();

    /**
     * @brief 添加任务计数器
     * @param count 要添加的数量
     */
    static void add_task_counter(size_t count);

    /**
     * @brief 获取任务计数器值
     * @return 当前计数器值
     */
    static size_t get_task_counter();

    /**
     * @brief 重置任务计数器
     */
    static void reset_task_counter();
};

}  // namespace oneday::common
