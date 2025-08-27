# Coding Standards

## Core Standards
- **Languages & Runtimes:** C++20标准，MSVC编译器，Qt6.6+框架
- **Style & Linting:** clang-format配置，遵循Google C++风格指南
- **Test Organization:** 测试文件与源文件并行目录结构，`_test.cpp`后缀

## Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Classes | PascalCase | BlueprintNode |
| Functions | camelCase | executeGraph |
| Variables | camelCase | nodeCount |
| Constants | UPPER_SNAKE_CASE | MAX_NODE_LIMIT |
| Files | snake_case | blueprint_engine.cpp |

## Critical Rules
- **Memory Management:** 优先使用智能指针，避免原始指针，RAII原则管理资源
- **Thread Safety:** 所有共享数据必须使用互斥锁保护，优先使用Qt的线程安全类
- **Error Handling:** 不使用异常处理控制流程，所有public函数必须有错误返回值
- **Performance Critical:** 避免在热路径中使用虚函数，优先使用栈分配而非堆分配
- **Qt Integration:** UI更新必须在主线程中进行，使用Qt信号槽进行跨线程通信
