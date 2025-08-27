# VSCode C++ 开发环境配置指南

## 📋 系统要求

- **操作系统**: Windows 10/11 (x64)
- **内存**: 至少8GB RAM (推荐16GB)
- **存储空间**: 至少10GB可用空间
- **网络**: 稳定的互联网连接

## 🛠️ 第一步：安装基础软件

### 1. Visual Studio Code
- **下载地址**: https://code.visualstudio.com/
- 选择Windows x64版本
- 安装时勾选"添加到PATH环境变量"

### 2. Visual Studio Build Tools 2022
- **下载地址**: https://visualstudio.microsoft.com/zh-hans/downloads/#build-tools-for-visual-studio-2022
- 或安装完整的Visual Studio 2022 Community
- **必需组件**:
  - MSVC v143 - VS 2022 C++ x64/x86生成工具
  - Windows 11 SDK (最新版本)
  - CMake Tools for Visual Studio

### 3. CMake (独立安装)
- **下载地址**: https://cmake.org/download/
- 选择Windows x64 Installer
- 安装时勾选"Add CMake to system PATH"

### 4. Git
- **下载地址**: https://git-scm.com/download/win
- 使用默认设置安装

## 📦 第二步：安装VSCode扩展

打开VSCode，按 `Ctrl+Shift+X` 打开扩展面板，安装以下扩展：

### 必需扩展
- **C/C++** (Microsoft) - C++语言支持
- **C/C++ Extension Pack** (Microsoft) - C++扩展包
- **CMake Tools** (Microsoft) - CMake集成
- **CMake** (twxs) - CMake语法高亮

### 推荐扩展
- **Qt tools** (tonka3000) - Qt开发支持
- **GitLens** (GitKraken) - Git增强功能
- **Error Lens** (Alexander) - 内联错误显示
- **Code Runner** (Jun Han) - 快速运行代码

## ⚙️ 第三步：配置vcpkg (如果尚未配置)

```cmd
# 克隆vcpkg到C盘根目录
cd C:\
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# 运行bootstrap脚本
.\bootstrap-vcpkg.bat

# 集成到系统
.\vcpkg.exe integrate install
```

**设置环境变量**:
1. 右键"此电脑" → "属性" → "高级系统设置" → "环境变量"
2. 在"系统变量"中添加:
   - 变量名: `VCPKG_ROOT`
   - 变量值: `C:\vcpkg`
3. 将 `C:\vcpkg` 添加到 PATH 环境变量

## 🚀 第四步：配置项目

### 1. 打开项目
- 启动VSCode
- 选择 `文件` → `打开文件夹`
- 选择项目根目录 `D:\dev\OneDay_Framework`

### 2. 安装推荐扩展
VSCode会自动提示安装推荐的扩展，点击"安装"即可。

### 3. 配置CMake
按 `Ctrl+Shift+P` 打开命令面板，输入并选择：
- `CMake: Configure` - 配置项目
- `CMake: Select Kit` - 选择编译器工具链

选择 `Visual Studio Community 2022 Release - amd64`

## 🔧 第五步：构建和运行

### 使用CMake Tools扩展
1. 按 `Ctrl+Shift+P` 打开命令面板
2. 输入 `CMake: Build` 构建项目
3. 输入 `CMake: Run Without Debugging` 运行程序

### 使用任务 (Tasks)
按 `Ctrl+Shift+P` 然后输入 `Tasks: Run Task`，选择：
- `CMake: Configure` - 配置项目
- `CMake: Build Debug` - 构建Debug版本
- `CMake: Build Release` - 构建Release版本
- `Run Tests` - 运行测试

### 使用快捷键
- `Ctrl+Shift+B` - 快速构建 (默认Debug)
- `F5` - 开始调试
- `Ctrl+F5` - 运行不调试

## 🐛 第六步：调试配置

### 设置断点
- 在代码行号左侧点击设置断点
- 红色圆点表示断点已设置

### 开始调试
- 按 `F5` 开始调试
- 选择调试配置：
  - `Debug Main Application` - 调试主程序
  - `Debug Unit Tests` - 调试单元测试
  - `Debug Integration Tests` - 调试集成测试

### 调试控制
- `F5` - 继续执行
- `F10` - 单步跳过
- `F11` - 单步进入
- `Shift+F11` - 单步跳出
- `Shift+F5` - 停止调试

## 📝 第七步：代码编辑功能

### IntelliSense
- 自动完成：输入时自动显示建议
- 参数提示：函数调用时显示参数信息
- 错误检查：实时显示语法错误

### 代码导航
- `Ctrl+Click` - 跳转到定义
- `F12` - 转到定义
- `Shift+F12` - 查找所有引用
- `Ctrl+Shift+O` - 转到符号

### 代码格式化
- `Shift+Alt+F` - 格式化整个文件
- `Ctrl+K Ctrl+F` - 格式化选中代码

## ✅ 验证安装

### 1. 检查编译器
打开VSCode终端 (`Ctrl+``)，运行：
```cmd
cl
cmake --version
```

### 2. 构建项目
```cmd
# 配置项目
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# 构建项目
cmake --build build --config Debug
```

### 3. 运行测试
```cmd
cd build
ctest -C Debug --output-on-failure
```

## 🔧 常见问题解决

### IntelliSense不工作
1. 按 `Ctrl+Shift+P`
2. 输入 `C/C++: Reset IntelliSense Database`
3. 重新加载窗口

### CMake配置失败
1. 检查vcpkg路径是否正确
2. 确认Visual Studio Build Tools已安装
3. 重新运行 `CMake: Configure`

### 找不到头文件
1. 检查 `.vscode/c_cpp_properties.json` 中的includePath
2. 确认依赖包已通过vcpkg安装
3. 重新生成CMake缓存

### 调试器无法启动
1. 确认程序已成功编译
2. 检查 `.vscode/launch.json` 中的程序路径
3. 确认Visual Studio Debugger已安装

## 🎯 下一步

环境配置完成后，你可以：

1. ✅ 开始编写C++代码
2. ✅ 使用IntelliSense进行智能编码
3. ✅ 设置断点进行调试
4. ✅ 运行单元测试和集成测试
5. ✅ 使用Git进行版本控制

**预计配置时间**: 30-60分钟  
**磁盘空间占用**: 5-8GB (不包括依赖包)

---

*配置过程中如有问题，请参考项目文档或查看VSCode输出面板的错误信息*
