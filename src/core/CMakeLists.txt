# 核心引擎模块
add_library(CoreEngine STATIC)

# 添加源文件 - 只包含存在的文件
target_sources(CoreEngine
    PRIVATE
    blueprint/engine.cpp
    blueprint/graph.cpp
    blueprint/data_types.cpp
    blueprint/execution_context.cpp
    blueprint/nodes/base_node.cpp
    blueprint/nodes/control_flow_nodes.cpp
    blueprint/nodes/logic_nodes.cpp
    blueprint/nodes/math_nodes.cpp
    blueprint/nodes/variable_nodes.cpp
    common/logger.cpp
    common/config.cpp
    common/utils.cpp
    common/encoding_utils.cpp
    common/parallel_utils.cpp
)

# 包含目录
target_include_directories(CoreEngine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

# 链接库
target_link_libraries(CoreEngine
    PUBLIC
    ${OpenCV_LIBS}
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    # 几何计算库 - 替代CGAL
    Boost::system
    Eigen3::Eigen
    # 并行计算库 - 标准库线程
    Threads::Threads
    # ONNX模型格式支持
    ONNX::onnx
)

# 编译特性
target_compile_features(CoreEngine PUBLIC cxx_std_20)

# 几何库编译定义
target_compile_definitions(CoreEngine
    PRIVATE
    # Boost.Geometry优化
    BOOST_GEOMETRY_INDEX_DETAIL_EXPERIMENTAL
    # Eigen3优化
    EIGEN_USE_MKL_ALL
    EIGEN_VECTORIZE_SSE4_2
    # 标准库并行优化
    ENABLE_STD_PARALLEL=1
)

# Windows特定设置
if(WIN32)
    target_link_libraries(CoreEngine PRIVATE user32 gdi32)
endif()