# OneDay Framework GitHub 配置指南

**版本**: v1.0.0  
**创建时间**: 2025-01-27  
**适用于**: 首次将项目推送到 GitHub

## 📋 前置准备

### 1. 确保已安装 Git
```powershell
git --version
# 应该显示: git version 2.x.x.windows.x
```

### 2. GitHub 账户准备
- 确保你有 GitHub 账户
- 记住你的 GitHub 用户名

## 🚀 快速配置 (推荐)

### 使用自动化脚本

1. **运行配置脚本**:
```powershell
.\setup_github.ps1 -GitHubUsername "your-github-username"
```

2. **按提示输入信息**:
   - Git 用户名 (用于提交记录)
   - Git 邮箱 (用于提交记录)
   - 确认文件添加
   - 确认推送到 GitHub

3. **脚本会自动完成**:
   - Git 仓库初始化
   - 远程仓库配置
   - 文件添加和提交
   - 推送到 GitHub

## 🔧 手动配置步骤

如果你想手动执行每个步骤：

### 步骤 1: 在 GitHub 创建仓库

1. 访问 [GitHub](https://github.com) 并登录
2. 点击右上角 "+" → "New repository"
3. 配置仓库:
   - **Repository name**: `OneDay_Framework`
   - **Description**: `无限蓝图游戏脚本开发框架 - 基于 C++20 和 Qt6`
   - **Public** (推荐，这样 CI/CD 免费)
   - **不要勾选** README, .gitignore, license (我们已经有了)
4. 点击 "Create repository"

### 步骤 2: 本地 Git 配置

```powershell
# 配置 Git 用户信息 (如果还没配置)
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"

# 初始化 Git 仓库
git init

# 添加远程仓库 (替换 your-username)
git remote add origin https://github.com/your-username/OneDay_Framework.git
```

### 步骤 3: 添加文件和提交

```powershell
# 检查将要添加的文件
git status

# 添加所有文件
git add .

# 创建初始提交
git commit -m "🎉 Initial commit: OneDay Framework with CI/CD

✅ 完整的项目架构和构建系统
✅ Qt6 + C++20 技术栈  
✅ 零警告的代码质量基线
✅ 完整的单元测试框架
✅ GitHub Actions CI/CD 流程
✅ 企业级开发基础设施"
```

### 步骤 4: 推送到 GitHub

```powershell
# 设置默认分支为 main
git branch -M main

# 推送到 GitHub
git push -u origin main
```

## 🔐 身份验证配置

### 方法 1: Personal Access Token (推荐)

1. **创建 Token**:
   - 访问 GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
   - 点击 "Generate new token (classic)"
   - 选择权限: `repo` (完整仓库访问)
   - 复制生成的 token

2. **使用 Token**:
   ```powershell
   # 第一次推送时，用户名使用 GitHub 用户名，密码使用 token
   git push -u origin main
   # Username: your-github-username
   # Password: ghp_xxxxxxxxxxxxxxxxxxxx (你的 token)
   ```

### 方法 2: SSH 密钥 (高级)

1. **生成 SSH 密钥**:
   ```powershell
   ssh-keygen -t ed25519 -C "your.email@example.com"
   ```

2. **添加到 GitHub**:
   - 复制 `~/.ssh/id_ed25519.pub` 内容
   - GitHub → Settings → SSH and GPG keys → New SSH key
   - 粘贴公钥内容

3. **使用 SSH URL**:
   ```powershell
   git remote set-url origin git@github.com:your-username/OneDay_Framework.git
   ```

## ✅ 验证配置

### 1. 检查仓库状态
访问 `https://github.com/your-username/OneDay_Framework`，应该看到:
- 所有项目文件已上传
- README.md 正确显示
- CI/CD 状态徽章显示

### 2. 检查 CI/CD 运行
1. 点击 "Actions" 标签页
2. 应该看到 "CI" workflow 正在运行或已完成
3. 点击查看详细日志

### 3. 测试推送流程
```powershell
# 做一个小改动测试
echo "# Test" >> test.md
git add test.md
git commit -m "test: Add test file"
git push

# 检查 GitHub Actions 是否自动触发
```

## 🔧 GitHub 仓库配置

### 1. 分支保护 (推荐)

1. 访问仓库 → Settings → Branches
2. 点击 "Add rule"
3. 配置:
   - **Branch name pattern**: `main`
   - ✅ **Require status checks to pass before merging**
   - ✅ **Require branches to be up to date before merging**
   - 选择 CI 检查项
   - ✅ **Restrict pushes that create files larger than 100 MB**

### 2. Secrets 配置 (如果需要)

如果 CI/CD 需要特殊配置:
1. 访问仓库 → Settings → Secrets and variables → Actions
2. 添加需要的 secrets

### 3. 仓库设置优化

1. **General 设置**:
   - ✅ Issues
   - ✅ Pull Requests  
   - ✅ Discussions (可选)

2. **Pages 设置** (如果要发布文档):
   - Source: GitHub Actions
   - 可以用于发布 API 文档

## 📋 日常 Git 工作流

### 基本命令

```powershell
# 查看状态
git status

# 查看更改
git diff

# 添加文件
git add .                    # 添加所有更改
git add src/specific_file.cpp # 添加特定文件

# 提交更改
git commit -m "feat: Add new feature"
git commit -m "fix: Fix bug in parser"
git commit -m "docs: Update README"

# 推送到远程
git push

# 拉取远程更改
git pull
```

### 提交消息规范

使用 [Conventional Commits](https://www.conventionalcommits.org/) 格式:

```
<type>(<scope>): <description>

[optional body]

[optional footer(s)]
```

**类型**:
- `feat`: 新功能
- `fix`: 错误修复
- `docs`: 文档更新
- `style`: 代码格式化
- `refactor`: 代码重构
- `test`: 测试相关
- `chore`: 构建过程或辅助工具的变动

**示例**:
```
feat(blueprint): Add node connection validation
fix(ui): Fix canvas zoom issue
docs(readme): Update build instructions
test(core): Add unit tests for graph module
```

## 🚨 常见问题解决

### 1. 推送被拒绝
```
error: failed to push some refs to 'https://github.com/...'
```
**解决方案**:
```powershell
git pull --rebase origin main
git push
```

### 2. 身份验证失败
```
remote: Support for password authentication was removed
```
**解决方案**: 使用 Personal Access Token 而不是密码

### 3. 文件太大
```
remote: error: File xxx is 123.45 MB; this exceeds GitHub's file size limit of 100.00 MB
```
**解决方案**: 
- 添加到 `.gitignore`
- 使用 Git LFS (大文件存储)

### 4. CI/CD 失败
- 检查 Actions 标签页的详细日志
- 确保所有依赖都在 `vcpkg.json` 中
- 检查代码是否有编译警告

## 🎯 下一步

配置完成后，你可以:

1. **开始开发**: 每次推送都会自动触发 CI/CD
2. **创建 Release**: 推送 `v1.0.0` 标签自动创建发布
3. **协作开发**: 邀请其他开发者，使用 Pull Request 流程
4. **监控质量**: 通过 Actions 页面监控构建和测试状态

## 📞 获取帮助

- [GitHub 官方文档](https://docs.github.com/)
- [Git 官方文档](https://git-scm.com/doc)
- [GitHub Actions 文档](https://docs.github.com/en/actions)
