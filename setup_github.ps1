# OneDay Framework GitHub 配置脚本
# 使用方法: .\setup_github.ps1 -GitHubUsername "your-username" -RepoName "OneDay_Framework"

param(
    [Parameter(Mandatory=$true)]
    [string]$GitHubUsername,
    
    [Parameter(Mandatory=$false)]
    [string]$RepoName = "OneDay_Framework",
    
    [Parameter(Mandatory=$false)]
    [string]$UserEmail = "",
    
    [Parameter(Mandatory=$false)]
    [string]$UserName = ""
)

Write-Host "🚀 OneDay Framework GitHub 配置开始..." -ForegroundColor Green

# 1. 检查 Git 配置
Write-Host "`n📋 步骤 1: 检查 Git 配置" -ForegroundColor Yellow

if ([string]::IsNullOrEmpty($UserName)) {
    $UserName = Read-Host "请输入你的 Git 用户名"
}

if ([string]::IsNullOrEmpty($UserEmail)) {
    $UserEmail = Read-Host "请输入你的 Git 邮箱"
}

# 配置 Git 用户信息
git config --global user.name "$UserName"
git config --global user.email "$UserEmail"

Write-Host "✅ Git 用户配置完成: $UserName <$UserEmail>" -ForegroundColor Green

# 2. 初始化 Git 仓库
Write-Host "`n📋 步骤 2: 初始化 Git 仓库" -ForegroundColor Yellow

if (!(Test-Path ".git")) {
    git init
    Write-Host "✅ Git 仓库初始化完成" -ForegroundColor Green
} else {
    Write-Host "ℹ️ Git 仓库已存在" -ForegroundColor Blue
}

# 3. 添加远程仓库
Write-Host "`n📋 步骤 3: 配置远程仓库" -ForegroundColor Yellow

$remoteUrl = "https://github.com/$GitHubUsername/$RepoName.git"
$sshUrl = "git@github.com:$GitHubUsername/$RepoName.git"

Write-Host "远程仓库 URL: $remoteUrl"

# 检查是否已有 origin
$existingOrigin = git remote get-url origin 2>$null
if ($existingOrigin) {
    Write-Host "⚠️ 远程仓库 origin 已存在: $existingOrigin" -ForegroundColor Yellow
    $replace = Read-Host "是否替换为新的远程仓库? (y/N)"
    if ($replace -eq "y" -or $replace -eq "Y") {
        git remote set-url origin $remoteUrl
        Write-Host "✅ 远程仓库已更新" -ForegroundColor Green
    }
} else {
    git remote add origin $remoteUrl
    Write-Host "✅ 远程仓库已添加" -ForegroundColor Green
}

# 4. 添加所有文件到 Git
Write-Host "`n📋 步骤 4: 添加文件到 Git" -ForegroundColor Yellow

# 检查 .gitignore 是否存在
if (!(Test-Path ".gitignore")) {
    Write-Host "⚠️ .gitignore 文件不存在，建议先创建" -ForegroundColor Yellow
}

# 显示将要添加的文件
Write-Host "将要添加的文件:"
git add --dry-run .

$confirm = Read-Host "确认添加这些文件? (Y/n)"
if ($confirm -ne "n" -and $confirm -ne "N") {
    git add .
    Write-Host "✅ 文件已添加到暂存区" -ForegroundColor Green
    
    # 显示状态
    git status --short
} else {
    Write-Host "❌ 文件添加已取消" -ForegroundColor Red
    exit 1
}

# 5. 创建初始提交
Write-Host "`n📋 步骤 5: 创建初始提交" -ForegroundColor Yellow

$commitMessage = "🎉 Initial commit: OneDay Framework with CI/CD

✅ 完整的项目架构和构建系统
✅ Qt6 + C++20 技术栈
✅ 零警告的代码质量基线
✅ 完整的单元测试框架
✅ GitHub Actions CI/CD 流程
✅ 企业级开发基础设施

- 核心引擎框架 (CoreEngine)
- 用户界面框架 (UserInterface)  
- 蓝图编辑器基础
- 自动化构建和测试
- 代码安全扫描
- 自动化发布流程"

git commit -m "$commitMessage"
Write-Host "✅ 初始提交已创建" -ForegroundColor Green

# 6. 推送到 GitHub
Write-Host "`n📋 步骤 6: 推送到 GitHub" -ForegroundColor Yellow

Write-Host "准备推送到: $remoteUrl"
$push = Read-Host "确认推送到 GitHub? (Y/n)"

if ($push -ne "n" -and $push -ne "N") {
    # 设置默认分支为 main
    git branch -M main
    
    # 推送到远程仓库
    Write-Host "正在推送到 GitHub..." -ForegroundColor Blue
    git push -u origin main
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ 推送成功!" -ForegroundColor Green
    } else {
        Write-Host "❌ 推送失败，可能需要身份验证" -ForegroundColor Red
        Write-Host "请检查:" -ForegroundColor Yellow
        Write-Host "1. GitHub 仓库是否已创建" -ForegroundColor Yellow
        Write-Host "2. 是否有推送权限" -ForegroundColor Yellow
        Write-Host "3. 是否需要配置 SSH 密钥或 Personal Access Token" -ForegroundColor Yellow
    }
} else {
    Write-Host "❌ 推送已取消" -ForegroundColor Red
}

# 7. 显示后续步骤
Write-Host "`n🎯 后续步骤:" -ForegroundColor Green
Write-Host "1. 访问你的 GitHub 仓库: https://github.com/$GitHubUsername/$RepoName"
Write-Host "2. 检查 Actions 标签页，CI/CD 工作流应该自动运行"
Write-Host "3. 如果需要，配置仓库设置 (分支保护、Secrets 等)"
Write-Host "4. 开始开发，每次推送都会触发自动构建和测试"

Write-Host "`n🔧 常用 Git 命令:" -ForegroundColor Blue
Write-Host "git status          # 查看状态"
Write-Host "git add .           # 添加所有更改"
Write-Host "git commit -m 'msg' # 提交更改"
Write-Host "git push            # 推送到远程"
Write-Host "git pull            # 拉取远程更改"

Write-Host "`n🎉 GitHub 配置完成!" -ForegroundColor Green
