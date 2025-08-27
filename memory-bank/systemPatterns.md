# σ₂: System Patterns

_v1.1 | Created: 2025-01-24 | Updated: 2025-01-27_
_Π: DEVELOPMENT | Ω: CONTEXT_

## 🏛️ Architecture Overview

OneDay Framework 采用分层架构设计，核心分为以下几个主要层次：

### 📱 Presentation Layer (UI)

- **Qt6 Widgets**: 主要用户界面框架
- **蓝图编辑器**: 可视化节点编辑器
- **工具栏和菜单**: 标准桌面应用界面

### 🧠 Business Logic Layer (Core)

- **脚本引擎**: 蓝图到可执行代码的转换
- **节点系统**: 可扩展的节点类型框架
- **执行调度器**: 脚本执行和任务管理

### 🔧 Service Layer

- **配置管理**: JSON 配置文件处理
- **日志系统**: spdlog 集成的日志服务
- **插件系统**: 可扩展的功能模块

### 💾 Data Layer

- **项目文件管理**: 蓝图项目的序列化/反序列化
- **资源管理**: 静态资源和模型文件管理

## 🔄 Key Design Patterns

### 1. Model-View-Controller (MVC)

- **Model**: 核心数据结构和业务逻辑
- **View**: Qt6 界面组件
- **Controller**: 用户交互处理

### 2. Observer Pattern

- 节点状态变化通知
- UI 更新机制

### 3. Factory Pattern

- 节点类型创建
- 插件实例化

### 4. Command Pattern

- 操作历史和撤销/重做功能

## 🔗 Component Dependencies

```
UI Layer
    ↓
Core Engine
    ↓
Service Layer
    ↓
External Libraries (Qt6, OpenCV, etc.)
```

## 📐 Design Decisions

- **语言选择**: C++20 - 现代 C++特性，高性能
- **GUI 框架**: Qt6 - 成熟的跨平台 GUI 解决方案
- **构建系统**: CMake - 标准的 C++构建工具
- **包管理**: vcpkg - Microsoft 推荐的 C++包管理器
