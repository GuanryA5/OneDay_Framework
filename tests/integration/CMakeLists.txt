# 集成测试
find_package(GTest REQUIRED)

# 集成测试可执行文件
add_executable(integration_tests)

# 添加测试源文件
target_sources(integration_tests
    PRIVATE
    blueprint_integration_test.cpp
)

# 包含目录
target_include_directories(integration_tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# 链接库
target_link_libraries(integration_tests
    PRIVATE
    CoreEngine
    UserInterface
    GTest::gtest
    GTest::gtest_main
    Qt6::Core
    Qt6::Widgets
)

# 添加到测试
add_test(
    NAME IntegrationTests
    COMMAND integration_tests
)