# Qt6用户界面模块
add_library(UserInterface STATIC)

# 添加源文件
target_sources(UserInterface
    PRIVATE
    main_window.cpp
    editors/blueprint_editor.cpp
    editors/blueprint_editor_window.cpp
    editors/blueprint_editor_window_minimal.cpp
    editors/map_editor.cpp
    editors/node_palette.cpp
    editors/property_panel.cpp
    canvas/infinite_canvas.cpp
    canvas/blueprint_node.cpp
    canvas/node_property.cpp
)

# 包含目录
target_include_directories(UserInterface
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接库
target_link_libraries(UserInterface
    PUBLIC
    Qt6::Core
    Qt6::Widgets
    CoreEngine
)

# 编译特性
target_compile_features(UserInterface PUBLIC cxx_std_20)

# Qt6特定设置
set_target_properties(UserInterface PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)