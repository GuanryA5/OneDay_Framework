# 性能测试
find_package(GTest REQUIRED)

# 性能测试可执行文件
add_executable(performance_tests)

# 添加测试源文件
target_sources(performance_tests
    PRIVATE
    blueprint_performance_test.cpp
)

# 包含目录
target_include_directories(performance_tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# 链接库
target_link_libraries(performance_tests
    PRIVATE
    CoreEngine
    GTest::gtest
    GTest::gtest_main
)

# 添加到测试
add_test(
    NAME PerformanceTests
    COMMAND performance_tests
)