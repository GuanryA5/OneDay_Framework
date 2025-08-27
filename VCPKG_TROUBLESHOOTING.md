# vcpkg 故障排除指南

## 📋 常见问题和解决方案

### 1. 包名不存在错误

**错误**: `error: qt6-base does not exist`

**原因**: vcpkg端口库过旧或包名已更改

**解决方案**:
```powershell
# 1. 更新vcpkg到最新版本
cd D:\dev\vcpkg
git pull
.\bootstrap-vcpkg.bat

# 2. 使用正确的包名
.\vcpkg.exe install qtbase[widgets]:x64-windows qttools:x64-windows
```

### 2. VCPKG_ROOT环境变量冲突

**警告**: `vcpkg is using detected vcpkg root D:\dev\vcpkg and ignoring mismatched VCPKG_ROOT environment value`

**解决方案**:
```powershell
# 方法1: 删除环境变量
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", $null, "User")
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", $null, "Machine")

# 方法2: 使用命令行参数
.\vcpkg.exe install qtbase:x64-windows --vcpkg-root=D:\dev\vcpkg
```

### 3. 权限不足错误

**错误**: `Access denied` 或权限相关错误

**解决方案**:
```powershell
# 以管理员身份运行PowerShell
# 右键PowerShell图标 → "以管理员身份运行"
```

### 4. 网络连接问题

**错误**: 下载超时或连接失败

**解决方案**:
```powershell
# 设置代理（如果需要）
.\vcpkg.exe install qtbase:x64-windows --x-debug --debug

# 或者使用国内镜像源（中科大）
git config --global url."https://mirrors.ustc.edu.cn/vcpkg/".insteadOf "https://github.com/Microsoft/vcpkg/"
```

### 5. 磁盘空间不足

**错误**: `No space left on device`

**解决方案**:
```powershell
# 清理vcpkg缓存
.\vcpkg.exe remove --outdated
.\vcpkg.exe clean

# 移动vcpkg到更大的磁盘
# 然后更新环境变量VCPKG_ROOT
```

## 🔧 包名映射表

| 旧包名 | 新包名 | 说明 |
|--------|--------|------|
| qt6-base | qtbase | Qt基础库 |
| qt6-tools | qttools | Qt开发工具 |
| onnxruntime | onnx | ONNX机器学习库 |
| tensorrt | N/A | TensorRT需手动安装 |
| opencv | opencv4 | OpenCV图像库 |
| json | nlohmann-json | JSON处理库 |

## ✅ 验证安装

运行验证脚本检查安装状态:
```powershell
.\check_environment.ps1
```

## 🆘 获取帮助

如果问题依然存在:

1. **查看vcpkg官方文档**: https://vcpkg.io/
2. **检查GitHub Issues**: https://github.com/Microsoft/vcpkg/issues
3. **运行详细调试**:
   ```powershell
   .\vcpkg.exe install qtbase:x64-windows --debug --verbose
   ```

## 📞 项目支持

如果是OneDay Framework特定问题，请:
- 运行 `.\check_environment.ps1` 获取环境报告
- 提供详细错误信息
- 包含操作系统版本和vcpkg版本信息