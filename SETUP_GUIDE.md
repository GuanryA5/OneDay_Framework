# OneDay Framework 开发环境安装指南

## 📋 系统要求

- **操作系统**: Windows 10/11 (x64)
- **内存**: 至少8GB RAM (推荐16GB)
- **存储空间**: 至少10GB可用空间
- **网络**: 稳定的互联网连接

## 🛠️ 必需工具安装

### 1. Visual Studio 2022 Community (免费)

**下载地址**: https://visualstudio.microsoft.com/zh-hans/vs/community/

**安装组件** (勾选以下工作负载):
- **使用C++的桌面开发**
- **使用C++的游戏开发** (可选，包含额外的C++工具)

**个人组件** (在"个人组件"选项卡中勾选):
- `MSVC v143 - VS 2022 C++ x64/x86生成工具`
- `Windows 11 SDK (最新版本)`
- `CMake Tools for Visual Studio`
- `用于Windows的Git`

### 2. Git (如果VS没有安装)

**下载地址**: https://git-scm.com/download/win
- 使用默认设置安装即可

### 3. vcpkg 包管理器

在命令提示符中执行:

```cmd
# 克隆vcpkg到C盘根目录
cd C:\
git clone https://github.com/Microsoft/vcpkg.git

# 进入vcpkg目录
cd vcpkg

# 运行bootstrap脚本
.\bootstrap-vcpkg.bat

# 集成到Visual Studio (可选但推荐)
.\vcpkg.exe integrate install
```

**设置环境变量** (重要!):
1. 右键 "此电脑" → "属性" → "高级系统设置" → "环境变量"
2. 在"系统变量"中添加:
   - 变量名: `VCPKG_ROOT`
   - 变量值: `C:\vcpkg`
3. 将 `C:\vcpkg` 添加到 PATH 环境变量

## 📦 项目依赖安装

### 方法1: 自动安装脚本 (推荐)

在项目根目录运行PowerShell脚本：

```powershell
# 完整安装所有依赖
.\install_dependencies.ps1

# 快速模式（跳过确认）
.\install_dependencies.ps1 -Fast

# 跳过Qt6（如果已安装）
.\install_dependencies.ps1 -SkipQt

# 跳过OpenCV（如果已安装）
.\install_dependencies.ps1 -SkipOpenCV

# 如果遇到执行策略限制，先运行：
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### 方法2: 手动安装

在命令提示符中（以管理员身份运行）：

```cmd
cd C:\vcpkg

# 安装Qt (大约2GB，需要较长时间)
.\vcpkg.exe install qtbase[widgets]:x64-windows qttools:x64-windows

# 安装OpenCV (大约500MB)
.\vcpkg.exe install opencv4[contrib]:x64-windows

# 安装ONNX Runtime (大约200MB)
.\vcpkg.exe install onnxruntime:x64-windows

# 安装TensorRT (仅NVIDIA GPU用户需要)
.\vcpkg.exe install tensorrt:x64-windows

# 安装其他依赖
.\vcpkg.exe install tbb:x64-windows
.\vcpkg.exe install gtest:x64-windows
.\vcpkg.exe install cgal:x64-windows
.\vcpkg.exe install nlohmann-json:x64-windows
.\vcpkg.exe install spdlog:x64-windows
```

**预计安装时间**: 30-60分钟（取决于网络速度）
**预计占用空间**: 3-4GB

## 🚀 项目构建

### 1. 克隆项目

```cmd
cd D:\dev
git clone <项目地址> OneDay_Framework
cd OneDay_Framework
```

### 2. CMake配置

```cmd
# 创建构建目录
mkdir build
cd build

# 配置项目 (使用vcpkg工具链)
cmake -S .. -B . -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DCMAKE_PREFIX_PATH=C:\vcpkg\installed\x64-windows

# 或者使用Visual Studio生成器
cmake -S .. -B . -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
```

### 3. 编译项目

```cmd
# 编译 Release 版本
cmake --build . --config Release

# 编译 Debug 版本  
cmake --build . --config Debug
```

### 4. 运行测试

```cmd
# 运行所有测试
ctest -C Release

# 或者直接运行测试可执行文件
.\bin\unit_tests.exe
.\bin\integration_tests.exe
```

## 🔧 Visual Studio 集成 (可选但推荐)

### 1. 打开Visual Studio 2022
### 2. 选择 "打开本地文件夹"
### 3. 选择项目根目录 `D:\dev\OneDay_Framework`
### 4. VS会自动检测CMake项目并配置

## 🐛 常见问题解决

### Qt6找不到
```cmd
# 确认Qt6安装
C:\vcpkg\vcpkg.exe list | findstr qt6
# 应该看到 qt6-base 和 qt6-tools
```

### OpenCV链接错误
```cmd
# 重新安装OpenCV
C:\vcpkg\vcpkg.exe remove opencv4:x64-windows
C:\vcpkg\vcpkg.exe install opencv4[contrib]:x64-windows
```

### CMake版本过低
- 更新Visual Studio 2022到最新版本
- 或手动安装 CMake 3.25+: https://cmake.org/download/

### vcpkg包找不到
```cmd
# 清理vcpkg缓存
C:\vcpkg\vcpkg.exe integrate remove
C:\vcpkg\vcpkg.exe integrate install

# 重新生成CMake缓存
cd build
del CMakeCache.txt
cmake -S .. -B . -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
```

## ✅ 验证安装

运行以下PowerShell脚本验证环境：

```powershell
# 在项目根目录运行环境验证脚本
.\check_environment.ps1

# 如果遇到执行策略限制，先运行：
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

脚本会检查：
- ✅ Visual Studio MSVC编译器 
- ✅ CMake版本 (需要3.25+)
- ✅ vcpkg安装状态
- ✅ Qt6、OpenCV等依赖包
- ✅ 项目文件结构

或手动检查各组件：

```cmd
# 检查编译器
cl

# 检查CMake
cmake --version

# 检查vcpkg
D:\dev\vcpkg\vcpkg.exe version

# 检查已安装的包
D:\dev\vcpkg\vcpkg.exe list
```

## 🎯 下一步

环境安装完成后，返回项目进行：

1. ✅ **Task 6.1**: 执行CMake配置
2. ✅ **Task 6.2**: 解决编译错误  
3. ✅ **Task 6.3**: 运行测试验证

**预计总安装时间**: 1-2小时  
**磁盘空间占用**: 8-10GB

---

*如有问题，请参考项目文档或提交Issue*