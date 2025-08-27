# OneDay Framework CI/CD 指南

**版本**: v1.0.0  
**创建时间**: 2025-01-27  
**维护者**: 开发团队

## 📋 概览

OneDay Framework 使用 GitHub Actions 实现完整的 CI/CD 流程，包括自动化构建、测试、质量检查和发布管理。

## 🔄 CI/CD 流程架构

### 持续集成 (CI)
```
代码推送 → 环境准备 → 依赖安装 → 编译构建 → 单元测试 → 质量检查 → 报告生成
```

### 持续部署 (CD)
```
标签推送 → 发布构建 → 测试验证 → 打包发布 → GitHub Release → 产物上传
```

## 🚀 Workflows 详解

### 1. CI Workflow (`.github/workflows/ci.yml`)

**触发条件**:
- Push to `main` 或 `develop` 分支
- Pull Request 到 `main` 或 `develop` 分支
- 手动触发

**执行步骤**:
1. **环境准备**
   - Windows Latest Runner
   - MSVC 2022 编译器
   - CMake 最新版本

2. **依赖管理**
   - vcpkg 缓存优化
   - 自动安装项目依赖

3. **矩阵构建**
   - Debug 和 Release 配置
   - 并行构建加速

4. **质量检查**
   - 零警告验证
   - 单元测试执行
   - 测试报告生成

**产物输出**:
- 测试结果 XML
- 构建产物 (Release)
- 质量报告

### 2. Release Workflow (`.github/workflows/release.yml`)

**触发条件**:
- 推送 `v*` 标签 (如 `v1.0.0`)
- 手动触发 (指定版本)

**执行步骤**:
1. **版本管理**
   - 自动提取版本号
   - 生成变更日志

2. **发布构建**
   - Release 配置构建
   - 完整测试验证

3. **打包发布**
   - 创建发布包
   - 包含所有依赖
   - 自动上传到 GitHub Releases

### 3. CodeQL Security Analysis (`.github/workflows/codeql.yml`)

**触发条件**:
- Push to `main` 或 `develop`
- Pull Request to `main`
- 每周定时扫描

**功能**:
- 静态代码安全分析
- 漏洞检测
- 代码质量评估

## 🛠️ 本地开发集成

### 预提交检查

建议在本地设置预提交钩子：

```powershell
# 检查构建
cmake --build build --config Release

# 运行测试
cd build/bin/Release
.\unit_tests.exe

# 检查格式 (如果有 clang-format)
# clang-format --dry-run --Werror src/**/*.cpp src/**/*.h
```

### 分支策略

推荐的 Git 分支策略：

```
main (生产)
├── develop (开发)
│   ├── feature/new-feature
│   ├── bugfix/fix-issue
│   └── hotfix/urgent-fix
└── release/v1.0.0
```

## 📊 状态徽章

在 README.md 中添加状态徽章：

```markdown
![CI](https://github.com/username/OneDay_Framework/workflows/CI/badge.svg)
![Release](https://github.com/username/OneDay_Framework/workflows/Release/badge.svg)
![CodeQL](https://github.com/username/OneDay_Framework/workflows/CodeQL/badge.svg)
```

## 🔧 配置管理

### 环境变量

| 变量名 | 描述 | 默认值 |
|--------|------|--------|
| `VCPKG_ROOT` | vcpkg 安装路径 | `${{ github.workspace }}/vcpkg` |
| `VCPKG_DEFAULT_TRIPLET` | 目标平台 | `x64-windows` |

### 缓存策略

- **vcpkg 缓存**: 基于 `vcpkg.json` 哈希
- **构建缓存**: 基于源代码变更
- **缓存生命周期**: 7 天自动清理

## 🚨 故障排除

### 常见问题

1. **vcpkg 安装失败**
   ```
   解决方案: 检查 vcpkg.json 配置，确保包名正确
   ```

2. **编译警告导致失败**
   ```
   解决方案: 修复所有编译警告，项目采用零警告策略
   ```

3. **测试超时**
   ```
   解决方案: 检查测试逻辑，优化测试性能
   ```

4. **依赖 DLL 缺失**
   ```
   解决方案: 确保所有运行时依赖都被正确复制
   ```

### 调试 Workflow

1. **启用调试日志**:
   ```yaml
   - name: Debug step
     run: |
       echo "::debug::Debug message"
   ```

2. **查看详细输出**:
   ```yaml
   - name: Verbose build
     run: |
       cmake --build build --config Release --verbose
   ```

3. **上传调试产物**:
   ```yaml
   - name: Upload debug info
     uses: actions/upload-artifact@v4
     with:
       name: debug-info
       path: build/debug/
   ```

## 📈 性能优化

### 构建加速

1. **并行构建**: `--parallel` 参数
2. **缓存优化**: 合理的缓存键设计
3. **增量构建**: 避免不必要的重新构建

### 资源使用

- **Runner 选择**: `windows-latest` 提供最佳性能
- **并发限制**: 合理控制并发任务数量
- **超时设置**: 防止无限等待

## 🔒 安全考虑

### Secrets 管理

- 使用 GitHub Secrets 存储敏感信息
- 定期轮换访问令牌
- 最小权限原则

### 代码安全

- CodeQL 自动扫描
- 依赖漏洞检查
- 安全编码规范

## 📋 维护清单

### 每月检查

- [ ] 更新 GitHub Actions 版本
- [ ] 检查 vcpkg 包更新
- [ ] 审查安全扫描结果
- [ ] 优化构建性能

### 每季度检查

- [ ] 评估 CI/CD 流程效率
- [ ] 更新文档和指南
- [ ] 检查缓存策略有效性
- [ ] 审查分支保护规则

## 🎯 未来改进

### 短期目标

- [ ] 添加代码覆盖率报告
- [ ] 集成更多静态分析工具
- [ ] 优化构建时间

### 长期目标

- [ ] 多平台构建支持 (Linux, macOS)
- [ ] 自动化性能基准测试
- [ ] 集成部署到测试环境

## 📞 支持

如有 CI/CD 相关问题，请：

1. 查看 [GitHub Actions 日志](https://github.com/username/OneDay_Framework/actions)
2. 创建 [Issue](https://github.com/username/OneDay_Framework/issues)
3. 参考 [GitHub Actions 文档](https://docs.github.com/en/actions)
