#include "parallel_utils.h"

#include <chrono>

#include "logger.h"

namespace oneday::common {

// ??????????????????????
// ????????????????
namespace {
// ????????
class PerformanceTimer {
  public:
    PerformanceTimer() : start_time_(std::chrono::high_resolution_clock::now()) {}

    double elapsed_ms() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        return duration.count() / 1000.0;
    }

  private:
    std::chrono::high_resolution_clock::time_point start_time_;
};
}  // namespace

// ???????????
void ParallelUtils::benchmark_parallel_performance() {
    oneday::core::Logger::info("Starting parallel computing performance benchmark...");

    const size_t test_size = 1000000;
    std::vector<int> test_data(test_size);

    // Initialize test data
    for (size_t i = 0; i < test_size; ++i) {
        test_data[i] = static_cast<int>(i);
    }

    // Test parallel for loop performance
    {
        PerformanceTimer timer;
        parallel_for(0, test_size, [&test_data](size_t i) { test_data[i] = test_data[i] * 2 + 1; });
        double elapsed = timer.elapsed_ms();
        oneday::core::Logger::info("Parallel for loop processed " + std::to_string(test_size) +
                                   " elements in: " + std::to_string(elapsed) + " ms");
    }

    // Test parallel reduce performance
    {
        PerformanceTimer timer;
        long long sum = parallel_reduce(test_data, 0LL, [](long long a, int b) { return a + b; });
        double elapsed = timer.elapsed_ms();
        oneday::core::Logger::info("Parallel reduce sum: " + std::to_string(sum) +
                                   ", time: " + std::to_string(elapsed) + " ms");
    }

    oneday::core::Logger::info("Parallel computing performance benchmark completed");
}

// System information logging
void ParallelUtils::log_system_info() {
    unsigned int cores = std::thread::hardware_concurrency();
    unsigned int recommended = getRecommendedThreadCount();

    oneday::core::Logger::info("=== Parallel Computing System Info ===");
    oneday::core::Logger::info("CPU cores: " + std::to_string(cores));
    oneday::core::Logger::info("Recommended worker threads: " + std::to_string(recommended));
    oneday::core::Logger::info(
        "C++17 parallel algorithm support: " +
        std::string(std::is_same_v<std::execution::parallel_unsequenced_policy,
                                   decltype(std::execution::par_unseq)>
                        ? "Yes"
                        : "No"));
    oneday::core::Logger::info("=======================================");
}

// Thread-safe counter (for debugging and monitoring)
class ThreadSafeCounter {
  public:
    void increment() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++count_;
    }

    void add(size_t value) {
        std::lock_guard<std::mutex> lock(mutex_);
        count_ += value;
    }

    size_t get() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        count_ = 0;
    }

  private:
    mutable std::mutex mutex_;
    size_t count_ = 0;
};

// Global counter instance (for performance monitoring)
static ThreadSafeCounter g_parallel_task_counter;

void ParallelUtils::increment_task_counter() {
    g_parallel_task_counter.increment();
}

void ParallelUtils::add_task_counter(size_t count) {
    g_parallel_task_counter.add(count);
}

size_t ParallelUtils::get_task_counter() {
    return g_parallel_task_counter.get();
}

void ParallelUtils::reset_task_counter() {
    g_parallel_task_counter.reset();
}

}  // namespace oneday::common
