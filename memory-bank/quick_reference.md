# σ₆: Quick Reference

_v1.0 | Created: 2025-01-27 | Updated: 2025-01-27_
_Π: DEVELOPMENT | Ω: CONTEXT_

## 🚀 常用命令 (Quick Commands)

### 🔨 构建命令 (Build Commands)
```powershell
# 自动安装依赖
.\install_dependencies.ps1

# 验证环境
.\check_environment.ps1

# 配置项目
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=D:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake

# 构建项目
cmake --build build --config Release

# 运行测试
cd build && ctest -C Release

# 快速构建
.\quick_build.ps1
```

### 📁 项目路径 (Project Paths)
- **项目根目录**: `d:\dev\OneDay_Framework`
- **源代码**: `src/`
- **构建目录**: `build/`
- **测试目录**: `tests/`
- **文档目录**: `docs/`
- **记忆库**: `memory-bank/`

### 🛠️ 开发工具 (Development Tools)
- **IDE**: Visual Studio 2022
- **编译器**: MSVC 2022
- **构建系统**: CMake 3.25+
- **包管理**: vcpkg
- **版本控制**: Git

## 📋 AugmentCRIPER♦Σ 快速参考

### 🔄 模式转换 (Mode Transitions)
- **Ω₀ = 🌀C (上下文)**: 收集和同步项目状态 `🚫代码修改`
- **Ω₁ = 🔍R (研究)**: 阅读、分析、探索 `🚫代码修改`
- **Ω₂ = 💡I (构想)**: 提出想法、评估方法 `🚫代码修改`
- **Ω₃ = 📝P (计划)**: 创建详细行动计划 `🚫代码修改`
- **Ω₄ = ⚙️E (执行)**: 编写和修改代码 `✅唯一允许修改代码`
- **Ω₅ = 🔎RV (审查)**: 测试、验证、检查 `🚫代码修改`

### 📚 记忆库文件 (Memory Files)
- **σ₁**: `project_brief.md` - 项目简报
- **σ₂**: `system_patterns.md` - 系统模式
- **σ₃**: `tech_context.md` - 技术背景
- **σ₄**: `dynamic_context.md` - 动态上下文
- **σ₅**: `progress_tracker.md` - 进度追踪
- **σ₆**: `quick_reference.md` - 速查手册

### ⚡️ 即时操作 (Instant Actions)
- **`/start`**: 项目启动协议
- **`/save`**: 会话归档协议

## 🔧 技术栈速查 (Tech Stack Quick Ref)

### 📦 核心依赖 (Core Dependencies)
```json
{
  "qtbase[widgets]": "GUI框架",
  "qttools": "Qt开发工具",
  "opencv4[contrib]": "计算机视觉",
  "onnx": "机器学习推理",
  "tbb": "并行计算",
  "gtest": "单元测试",
  "cgal": "计算几何",
  "nlohmann-json": "JSON处理",
  "spdlog": "日志系统"
}
```

### 🎯 关键文件 (Key Files)
- **CMakeLists.txt**: 构建配置
- **vcpkg.json**: 依赖定义
- **src/main.cpp**: 应用入口
- **src/core/**: 核心引擎
- **src/ui/**: 用户界面

## 📊 项目状态速查 (Project Status Quick View)

### ✅ 已完成 (Completed)
- 项目架构设计
- 构建系统配置
- 依赖管理设置
- 编译环境验证 (7/7 通过)
- AugmentCRIPER♦Σ 框架建立

### 🔄 进行中 (In Progress)
- 记忆库标准化
- 项目状态同步

### ⏳ 待开始 (Pending)
- 核心蓝图引擎开发
- Qt6 主窗口实现
- 单元测试编写
- 集成测试开发

## 🚨 重要提醒 (Important Notes)

### ⚠️ 安全协议 (Safety Protocols)
- **Δ₅**: 严格禁止在非执行(Ω₄)模式下修改代码
- **破坏性操作**: 需要警告和确认
- **阶段切换**: 需要验证条件

### 🎯 执行模式准则 (Execution Codex)
1. **防止蟑螂效应**: 不允许劣质临时代码
2. **KISS原则**: 保持简单直观
3. **高内聚低耦合**: 单一职责原则
4. **文件大小限制**: 300-500行上限
5. **可读性优先**: 清晰命名和注释
6. **避免随意创建文件**: 除非必要不创建临时文件
