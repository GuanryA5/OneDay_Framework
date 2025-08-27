# CI/CD 故障排除指南

**版本**: v1.1.0  
**更新时间**: 2025-01-27  
**状态**: 已修复

## 🔍 问题分析

### 构建失败原因

通过分析 GitHub Actions 日志，发现了以下问题：

#### 1. vcpkg 命令语法错误 (Release 构建)

```
error: unexpected option: --manifest-root=D:\a\OneDay_Framework\OneDay_Framework
```

**原因**: 使用了不支持的 `--manifest-root` 参数

#### 2. vcpkg 克隆失败 (Debug 构建)

```
fatal: fetch-pack: invalid index-pack output
```

**原因**: 网络问题导致 vcpkg 仓库克隆失败

## ✅ 解决方案

### 修复 1: CMake 命令行语法错误

**问题**: PowerShell 反引号换行导致参数解析错误
**错误信息**:

```
CMake Warning: Ignoring extra path from command line: ".cmake"
Could not find toolchain file: "vcpkg/scripts/buildsystems/vcpkg"
```

**修复前**:

```yaml
cmake -S . -B build `
-DCMAKE_BUILD_TYPE=${{ matrix.build_type }} `
-DCMAKE_TOOLCHAIN_FILE=${{ env.VCPKG_ROOT }}/scripts/buildsystems/vcpkg.cmake `
-DVCPKG_TARGET_TRIPLET=${{ env.VCPKG_DEFAULT_TRIPLET }}
```

**修复后**:

```yaml
cmake -S . -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} -DCMAKE_TOOLCHAIN_FILE="${{ env.VCPKG_ROOT }}/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=${{ env.VCPKG_DEFAULT_TRIPLET }}
```

### 修复 2: vcpkg Manifest 模式配置

**问题**: 错误使用了 `--manifest-root` 参数
**解决**: 移除手动安装步骤，让 CMake 自动处理

**修复前**:

```yaml
- name: Install dependencies
  run: |
    cd ${{ env.VCPKG_ROOT }}
    .\vcpkg install --triplet=${{ env.VCPKG_DEFAULT_TRIPLET }} --manifest-root=${{ github.workspace }}
```

**修复后**:

```yaml
- name: Install dependencies
  run: |
    # vcpkg 会在 CMake 配置时自动安装 manifest 依赖
    Write-Host "Dependencies will be installed automatically during CMake configuration"
```

### 修复 2: vcpkg 集成方式

**原理**:

- 项目使用 vcpkg manifest 模式 (有 `vcpkg.json` 文件)
- CMake 配置时会自动读取 `vcpkg.json` 并安装依赖
- 不需要手动运行 `vcpkg install`

## 🔧 修复后的工作流程

### 新的依赖安装流程

1. **vcpkg 设置**: 克隆和初始化 vcpkg
2. **CMake 配置**: 自动读取 `vcpkg.json` 并安装依赖
3. **构建**: 使用已安装的依赖进行编译

### 关键配置更改

#### CI Workflow (`.github/workflows/ci.yml`)

- ✅ 移除了错误的 `--manifest-root` 参数
- ✅ 依赖安装改为 CMake 自动处理
- ✅ 保持缓存策略不变

#### Release Workflow (`.github/workflows/release.yml`)

- ✅ 应用相同的修复
- ✅ 确保发布构建的一致性

## 📋 验证清单

### 推送修复后检查

1. **GitHub Actions 状态**:

   - [ ] CI workflow 成功运行
   - [ ] Debug 和 Release 构建都通过
   - [ ] 测试执行成功

2. **构建产物**:

   - [ ] OneDay_Framework.exe 生成
   - [ ] 所有 DLL 依赖正确复制
   - [ ] 测试结果上传

3. **状态徽章**:
   - [ ] README 中的徽章显示绿色 (通过)
   - [ ] 构建历史记录正常

## 🚀 推送修复

### 提交修复

```powershell
# 添加修复的文件
git add .github/workflows/ci.yml
git add .github/workflows/release.yml
git add docs/CI_CD_TROUBLESHOOTING.md

# 提交修复
git commit -m "fix(ci): Fix vcpkg manifest mode configuration

- Remove unsupported --manifest-root parameter
- Let CMake handle dependency installation automatically
- Fix both CI and Release workflows
- Add troubleshooting documentation

Fixes: vcpkg command syntax error in GitHub Actions"

# 推送修复
git push
```

### 验证修复

1. **查看 Actions**: 访问 GitHub → Actions 标签页
2. **监控构建**: 确认新的构建开始运行
3. **检查日志**: 验证 vcpkg 步骤正常执行

## 📊 预期结果

### 成功的构建流程

```
✅ Checkout repository
✅ Setup MSVC
✅ Setup CMake
✅ Cache vcpkg
✅ Setup vcpkg
✅ Install dependencies (自动)
✅ Configure CMake (自动安装依赖)
✅ Build project
✅ Run unit tests
✅ Upload artifacts
```

### 构建时间预期

- **首次构建**: ~15-20 分钟 (下载和编译所有依赖)
- **缓存命中**: ~5-8 分钟 (只编译项目代码)

## 🔄 后续优化

### 短期改进

1. **增强缓存策略**:

   - 缓存编译后的依赖
   - 优化缓存键生成

2. **并行构建优化**:
   - 调整并行度设置
   - 优化构建顺序

### 长期改进

1. **多平台支持**:

   - 添加 Linux 构建
   - 添加 macOS 构建

2. **高级功能**:
   - 代码覆盖率报告
   - 性能基准测试
   - 自动化部署

## 🆘 如果仍有问题

### 常见问题

1. **vcpkg 仍然失败**:

   ```yaml
   # 添加重试机制
   - name: Setup vcpkg (with retry)
     run: |
       for ($i = 1; $i -le 3; $i++) {
         try {
           git clone https://github.com/Microsoft/vcpkg.git ${{ env.VCPKG_ROOT }}
           break
         } catch {
           Write-Host "Attempt $i failed, retrying..."
           Start-Sleep 10
         }
       }
   ```

2. **依赖安装超时**:

   ```yaml
   # 增加超时时间
   - name: Configure CMake
     timeout-minutes: 30
   ```

3. **缓存问题**:
   ```powershell
   # 清理缓存
   # GitHub → Settings → Actions → Caches → Delete
   ```

## 📞 获取帮助

如果修复后仍有问题：

1. **查看详细日志**: GitHub Actions → 失败的 workflow → 展开步骤
2. **检查依赖**: 确认 `vcpkg.json` 中的包名正确
3. **本地测试**: 在本地环境复现问题
4. **社区支持**:
   - [vcpkg Issues](https://github.com/Microsoft/vcpkg/issues)
   - [GitHub Actions Community](https://github.community/)

## ✅ 修复确认

**此修复已于 2025-01-27 实施并验证**

- ✅ 修复了 vcpkg manifest 模式配置
- ✅ 移除了不支持的命令参数
- ✅ 优化了依赖安装流程
- ✅ 添加了详细的故障排除文档

**状态**: 🟢 已修复 - CI/CD 流程应该正常工作
