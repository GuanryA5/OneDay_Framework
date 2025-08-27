# Tech Stack

## Cloud Infrastructure

- **Provider:** 本地桌面应用，无云服务依赖
- **Key Services:** Windows本地服务（文件系统、注册表、DirectX/OpenGL）
- **Deployment Regions:** Windows 10/11桌面环境

## Technology Stack Table

| Category | Technology | Version | Purpose | Rationale |
|----------|------------|---------|---------|-----------|
| **Language** | C++ | 20 | 核心开发语言 | 高性能、内存控制、成熟生态 |
| **GUI Framework** | Qt | 6.6+ | 用户界面开发 | 成熟的跨平台GUI、Graphics View优秀 |
| **Build System** | CMake | 3.25+ | 项目构建管理 | 现代C++支持、跨平台、依赖管理 |
| **Package Manager** | vcpkg | latest | 第三方库管理 | Microsoft官方支持、C++生态完善 |
| **Image Processing** | OpenCV | 4.8+ | 图像处理核心 | 业界标准、性能优秀、文档完善 |
| **AI Inference** | ONNX Runtime | 1.16+ | AI模型推理 | 跨平台、多硬件后端支持 |
| **GPU Acceleration** | TensorRT | 8.6+ | NVIDIA GPU加速 | 专门优化AI推理性能 |
| **Parallel Computing** | Intel TBB | 2021.10+ | 并行计算框架 | 充分利用多核CPU资源 |
| **Testing Framework** | Google Test | 1.14+ | 单元测试 | C++标准测试框架 |
| **Geometry Algorithms** | CGAL | 5.5+ | 计算几何 | 高质量空间数据结构 |
| **JSON Processing** | nlohmann/json | 3.11+ | 配置文件处理 | 现代C++ JSON库 |
| **Logging** | spdlog | 1.12+ | 日志记录 | 高性能、多输出格式 |
| **Game Interface** | Windows API | Win32/Win64 | 系统级操作 | 鼠标键盘模拟、窗口管理 |
