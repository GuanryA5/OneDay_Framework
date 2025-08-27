# OneDay Framework

无限蓝图游戏脚本开发框架 - 构建系统已验证 ✅

[![CI](https://github.com/username/OneDay_Framework/workflows/CI/badge.svg)](https://github.com/username/OneDay_Framework/actions/workflows/ci.yml)
[![Release](https://github.com/username/OneDay_Framework/workflows/Release/badge.svg)](https://github.com/username/OneDay_Framework/actions/workflows/release.yml)
[![CodeQL](https://github.com/username/OneDay_Framework/workflows/CodeQL/badge.svg)](https://github.com/username/OneDay_Framework/actions/workflows/codeql.yml)

## 项目概述

OneDay Framework 是一个基于 C++20 和 Qt6 的游戏脚本开发框架，提供可视化蓝图编辑和自动化脚本执行能力。

## 技术栈

- **核心语言**: C++20
- **GUI 框架**: Qt 6.6+
- **构建系统**: CMake 3.25+
- **包管理器**: vcpkg
- **测试框架**: Google Test 1.14+
- **日志系统**: spdlog 1.12+
- **配置管理**: nlohmann/json 3.11+

## 项目结构

```
OneDay_Framework/
├── src/                    # 核心源代码
│   ├── core/              # 核心引擎模块
│   ├── ui/                # Qt6用户界面
│   └── main.cpp           # 应用程序入口
├── tests/                 # 测试代码
│   ├── unit/             # 单元测试
│   ├── integration/      # 集成测试
│   └── performance/      # 性能测试
├── docs/                  # 项目文档
├── assets/                # 静态资源
├── scripts/               # 构建脚本
├── third_party/           # 第三方库
└── CMakeLists.txt         # CMake配置
```

## 构建说明

> **📋 完整环境安装指南**: 请参考 [SETUP_GUIDE.md](./SETUP_GUIDE.md)

### 前置条件

1. Visual Studio 2022 (MSVC 编译器)
2. CMake 3.25+
3. vcpkg 包管理器
4. Git

### 快速构建步骤

```powershell
# 1. 自动安装依赖 (推荐)
.\install_dependencies.ps1

# 2. 验证环境
.\check_environment.ps1

# 3. 配置项目
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=D:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake

# 4. 构建项目
cmake --build build --config Release

# 5. 运行测试
cd build && ctest -C Release
```

### 手动安装依赖

```bash
# 安装核心依赖
vcpkg install qtbase[widgets]:x64-windows qttools:x64-windows
vcpkg install opencv4[contrib]:x64-windows onnx:x64-windows
vcpkg install tbb:x64-windows gtest:x64-windows cgal:x64-windows
vcpkg install nlohmann-json:x64-windows spdlog:x64-windows

# 注意: TensorRT需要从NVIDIA手动下载安装
# https://developer.nvidia.com/tensorrt
```

## 🚀 构建状态

### ✅ 已完成

- ✅ 项目基础架构
- ✅ CMake 构建系统
- ✅ vcpkg 依赖管理
- ✅ 核心模块框架 (CoreEngine.lib)
- ✅ 用户界面框架 (UserInterface.lib)
- ✅ 测试框架集成 (unit_tests.exe)
- ✅ **编译验证完成** - 所有核心组件构建成功
- ✅ **代码质量** - 0 编译警告，0 MOC 警告
- ✅ **应用程序** - OneDay_Framework.exe 可正常启动

### 📊 构建验证报告

详细的构建验证结果请查看: [BUILD_VERIFICATION_REPORT.md](docs/BUILD_VERIFICATION_REPORT.md)

**最后验证时间**: 2025-01-27
**构建环境**: Windows 11, MSVC 2022, CMake 4.1.0

## 🔄 CI/CD 流程

### 自动化构建

- ✅ **持续集成**: 每次代码推送自动构建和测试
- ✅ **多配置支持**: Debug 和 Release 并行构建
- ✅ **质量门禁**: 零警告策略，所有测试必须通过
- ✅ **安全扫描**: CodeQL 自动安全分析

### 自动化发布

- ✅ **版本管理**: 基于 Git 标签自动发布
- ✅ **产物打包**: 自动打包可执行文件和依赖
- ✅ **GitHub Releases**: 自动创建发布页面
- ✅ **变更日志**: 自动生成发布说明

详细的 CI/CD 配置和使用指南请查看: [CI_CD_GUIDE.md](docs/CI_CD_GUIDE.md)

## 许可证

待定

## 贡献指南

请参考 `docs/` 目录下的开发文档。
