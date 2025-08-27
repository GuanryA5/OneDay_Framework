cmake_minimum_required(VERSION 3.25)

project(OneDay_Framework
    VERSION 1.0.0
    DESCRIPTION "无限蓝图游戏脚本开发框架"
    LANGUAGES CXX
)

# 设置C++20标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译器特定设置
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")  # 降低警告级别
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
    # 启用多处理器编译
    add_compile_options(/MP)

    # 编码设置 - 解决中文字符问题
    add_compile_options(/utf-8)  # 源文件和执行字符集都使用UTF-8
    add_compile_definitions(
        UNICODE          # 启用Unicode API
        _UNICODE         # 启用Unicode CRT
        NOMINMAX         # 避免Windows.h定义min/max宏
        WIN32_LEAN_AND_MEAN  # 减少Windows.h包含内容
        _CRT_SECURE_NO_WARNINGS  # 禁用CRT安全警告
        _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING  # 禁用codecvt警告
    )
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# vcpkg集成
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()


# 查找依赖包
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(OpenCV REQUIRED)
# ONNX - 模型格式支持
find_package(ONNX REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(GTest REQUIRED)

# 几何计算库 - 替代CGAL
find_package(Boost REQUIRED COMPONENTS system)
find_package(Eigen3 REQUIRED)

# 并行计算库 - 使用标准库线程
find_package(Threads REQUIRED)

# 输出依赖信息
message(STATUS "=== OneDay Framework Dependencies ===")
message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "OpenCV Version: ${OpenCV_VERSION}")
message(STATUS "ONNX Found: ${ONNX_FOUND}")
message(STATUS "Boost Version: ${Boost_VERSION}")
message(STATUS "Eigen3 Version: ${Eigen3_VERSION}")
message(STATUS "Threads Found: ${Threads_FOUND}")
message(STATUS "CGAL Replacement: Boost.Geometry + Eigen3")
message(STATUS "Parallel Computing: Standard Library Threads")
message(STATUS "AI Support: ONNX Model Format")
message(STATUS "=====================================")

# 禁用MKL支持
add_definitions(-DEIGEN_DONT_USE_MKL)

# 性能优化选项
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release build - enabling optimizations")
    if(MSVC)
        add_compile_options(/O2 /Ob2 /Oi /Ot /Oy /GL)
        add_link_options(/LTCG)
    endif()
endif()

# 包含目录
include_directories(src)

# 添加子目录
add_subdirectory(src/core)
add_subdirectory(src/ui)
# 启用测试模块
add_subdirectory(tests)

# 主可执行文件
add_executable(${PROJECT_NAME} src/main.cpp)

# 链接库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    CoreEngine
    UserInterface
    Qt6::Core
    Qt6::Widgets
)

# 设置项目属性
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# 自动复制Qt和其他依赖DLL文件
if(WIN32)
    # 设置vcpkg依赖路径
    set(VCPKG_BIN_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/bin")
    set(VCPKG_PLUGINS_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/Qt6/plugins")

    # 定义需要复制的DLL文件列表
    set(REQUIRED_DLLS
        # Qt核心库
        "Qt6Core.dll"
        "Qt6Gui.dll"
        "Qt6Widgets.dll"
        "Qt6Network.dll"
        "Qt6OpenGL.dll"
        "Qt6OpenGLWidgets.dll"
        "Qt6PrintSupport.dll"

        # ICU国际化库
        "icudt74.dll"
        "icuin74.dll"
        "icuuc74.dll"

        # 其他依赖库
        "double-conversion.dll"
        "pcre2-16.dll"
        "zlib1.dll"
        "libpng16.dll"
        "harfbuzz.dll"
        "freetype.dll"
        "brotlicommon.dll"
        "brotlidec.dll"
        "bz2.dll"

        # OpenSSL库
        "libcrypto-3-x64.dll"
        "libssl-3-x64.dll"

        # 日志库
        "spdlog.dll"
        "fmt.dll"
    )

    # 添加后构建步骤来复制DLL文件
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "Copying Qt and dependency DLL files..."

        # 复制DLL文件
        COMMAND ${CMAKE_COMMAND} -E echo "Copying DLL files from ${VCPKG_BIN_DIR}"
    )

    # 为每个DLL添加复制命令
    foreach(DLL_FILE ${REQUIRED_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${VCPKG_BIN_DIR}/${DLL_FILE}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
            COMMENT "Copying ${DLL_FILE}"
        )
    endforeach()

    # 复制Qt插件
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "Copying Qt plugins..."

        # 创建插件目录
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/imageformats"
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/styles"

        # 复制平台插件
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${VCPKG_PLUGINS_DIR}/platforms/qwindows.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms/"

        # 复制图像格式插件
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${VCPKG_PLUGINS_DIR}/imageformats/qgif.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/imageformats/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${VCPKG_PLUGINS_DIR}/imageformats/qico.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/imageformats/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${VCPKG_PLUGINS_DIR}/imageformats/qjpeg.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/imageformats/"

        # 复制样式插件
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${VCPKG_PLUGINS_DIR}/styles/qmodernwindowsstyle.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/styles/"
    )

    # 输出完成信息
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "DLL copying completed successfully!"
    )
endif()