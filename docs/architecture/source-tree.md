# Source Tree

```
OneDay_Framework/
├── src/                           # 核心源代码
│   ├── core/                      # 核心引擎模块
│   │   ├── blueprint/             # 蓝图引擎
│   │   │   ├── engine.cpp/h       # 执行引擎
│   │   │   ├── nodes/             # 节点类型实现
│   │   │   └── graph.cpp/h        # 图结构管理
│   │   ├── image/                 # 图像处理模块
│   │   │   ├── processor.cpp/h    # 图像处理器
│   │   │   ├── models/            # AI模型管理
│   │   │   └── opencv_wrapper.cpp/h
│   │   ├── pathfinding/           # 寻路算法模块
│   │   │   ├── astar.cpp/h        # A*算法实现
│   │   │   ├── map.cpp/h          # 地图数据结构
│   │   │   └── pathplanner.cpp/h  # 路径规划器
│   │   ├── game/                  # 游戏接口模块
│   │   │   ├── window_manager.cpp/h # 窗口管理
│   │   │   ├── screen_capture.cpp/h # 截图功能
│   │   │   └── input_simulator.cpp/h # 输入模拟
│   │   └── common/                # 共享工具类
│   │       ├── logger.cpp/h       # 日志系统
│   │       ├── config.cpp/h       # 配置管理
│   │       └── utils.cpp/h        # 工具函数
│   ├── ui/                        # Qt6用户界面
│   │   ├── main_window.cpp/h      # 主窗口
│   │   ├── editors/               # 编辑器组件
│   │   │   ├── blueprint_editor.cpp/h # 蓝图编辑器
│   │   │   ├── map_editor.cpp/h   # 地图编辑器
│   │   │   └── node_palette.cpp/h # 节点面板
│   │   ├── widgets/               # 自定义控件
│   │   └── resources/             # 资源文件
│   └── main.cpp                   # 应用程序入口
├── tests/                         # 测试代码
│   ├── unit/                      # 单元测试
│   ├── integration/               # 集成测试
│   └── performance/               # 性能测试
├── docs/                          # 项目文档
├── assets/                        # 静态资源
│   ├── models/                    # AI模型文件
│   ├── icons/                     # 图标资源
│   └── templates/                 # 模板文件
├── scripts/                       # 构建脚本
├── third_party/                   # 第三方库
├── CMakeLists.txt                 # CMake配置
├── vcpkg.json                     # 包依赖配置
└── README.md                      # 项目说明
```
