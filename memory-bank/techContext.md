# σ₃: Technical Context

_v1.1 | Created: 2025-01-24 | Updated: 2025-01-27_
_Π: DEVELOPMENT | Ω: CONTEXT_

## 🛠️ Technology Stack

### 🖥️ Frontend

- **Qt 6.6+**: 主要 GUI 框架
  - QtWidgets: 传统桌面控件
  - QtCore: 核心功能模块
  - QtTools: 开发工具支持

### ⚙️ Backend/Core

- **C++20**: 核心开发语言
  - 现代 C++特性支持
  - 强类型系统
  - 高性能计算能力

### 🧠 AI/ML Integration

- **OpenCV 4**: 计算机视觉库
  - 图像处理功能
  - contrib 模块支持
- **ONNX**: 机器学习模型推理
  - 跨平台模型支持
- **TensorRT**: NVIDIA GPU 加速推理
  - 高性能推理引擎

### 🚀 Performance

- **Intel TBB**: 并行计算库
  - 多线程任务调度
  - 高性能并行算法

### 🔧 Development Tools

- **CMake 3.25+**: 构建系统
- **vcpkg**: 包管理器
- **Google Test 1.14+**: 单元测试框架
- **spdlog 1.12+**: 日志系统
- **nlohmann/json 3.11+**: JSON 处理

### 🏗️ Build Environment

- **Visual Studio 2022**: 主要开发环境
- **MSVC 编译器**: Windows 平台编译器
- **Git**: 版本控制系统

## 🌐 Platform Support

- **主要平台**: Windows x64
- **编译器**: MSVC 2022
- **最低系统要求**: Windows 10/11

## 📦 Dependencies Management

```json
{
  "name": "oneday-framework",
  "version": "0.1.0",
  "dependencies": [
    "qtbase[widgets]",
    "qttools",
    "opencv4[contrib]",
    "onnx",
    "tbb",
    "gtest",
    "cgal",
    "nlohmann-json",
    "spdlog"
  ]
}
```

## 🔄 Development Workflow

1. **代码编写**: Visual Studio 2022
2. **依赖管理**: vcpkg 自动安装
3. **构建**: CMake + Ninja
4. **测试**: Google Test 框架
5. **调试**: Visual Studio 调试器

## 📊 Performance Considerations

- **内存管理**: 智能指针和 RAII
- **并发处理**: Intel TBB 任务调度
- **GPU 加速**: TensorRT 推理优化
- **编译优化**: Release 模式优化标志
