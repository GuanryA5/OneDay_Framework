# 单元测试
find_package(GTest REQUIRED)
include(GoogleTest)

# 测试可执行文件
add_executable(unit_tests)

# 添加测试源文件 - 从最简单的开始
target_sources(unit_tests
    PRIVATE
    simple_test.cpp
    # 逐步启用其他测试
    # encoding_utils_test.cpp
    # core/common/logger_test.cpp
    # core/common/config_test.cpp
    # core/blueprint/engine_test.cpp
    # core/blueprint/graph_test.cpp
    # core/common/parallel_utils_test.cpp
    # core/pathfinding/geometry_utils_test.cpp
    # core/ai/onnx_model_test.cpp
)

# 包含目录
target_include_directories(unit_tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# 链接库 - 不使用 gtest_main，因为我们有自己的 main 函数
target_link_libraries(unit_tests
    PRIVATE
    CoreEngine
    GTest::gtest
    GTest::gmock
)

# 手动添加测试
add_test(
    NAME UnitTests
    COMMAND unit_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Release
)