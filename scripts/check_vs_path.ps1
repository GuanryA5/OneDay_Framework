# Visual Studio 路径检测脚本
Write-Host "=== Visual Studio 路径检测工具 ===" -ForegroundColor Cyan
Write-Host ""

# 手动检测Visual Studio路径
$possiblePaths = @(
    "C:\Program Files\Microsoft Visual Studio\2022\Professional",
    "C:\Program Files\Microsoft Visual Studio\2022\Community", 
    "C:\Program Files\Microsoft Visual Studio\2022\Enterprise",
    "D:\Applications\Microsoft Visual Studio\2022\Professional",
    "D:\Applications\Microsoft Visual Studio\2022\Community",
    "D:\Applications\Microsoft Visual Studio\2022\Enterprise"
)

$vsPath = $null
foreach ($path in $possiblePaths) {
    $vcToolsPath = Join-Path $path "VC\Tools\MSVC"
    if (Test-Path $vcToolsPath) {
        Write-Host "✅ 找到Visual Studio: $path" -ForegroundColor Green
        $vsPath = $path
        break
    }
}

if (-not $vsPath) {
    Write-Host "❌ 未找到Visual Studio 2022安装" -ForegroundColor Red
    exit 1
}

# 获取MSVC编译器路径
$vcToolsPath = Join-Path $vsPath "VC\Tools\MSVC"

if (-not (Test-Path $vcToolsPath)) {
    Write-Host "❌ 未找到VC Tools目录: $vcToolsPath" -ForegroundColor Red
    exit 1
}

# 找到最新版本的MSVC
$msvcVersions = Get-ChildItem $vcToolsPath | Where-Object { $_.PSIsContainer } | Sort-Object Name -Descending

if (-not $msvcVersions) {
    Write-Host "❌ 未找到MSVC版本目录" -ForegroundColor Red
    exit 1
}

$latestVersion = $msvcVersions[0].Name
$compilerPath = Join-Path $vcToolsPath "$latestVersion\bin\Hostx64\x64\cl.exe"

if (Test-Path $compilerPath) {
    Write-Host "✅ 找到编译器: $compilerPath" -ForegroundColor Green
} else {
    Write-Host "❌ 未找到编译器: $compilerPath" -ForegroundColor Red
    exit 1
}

# 显示检测结果
Write-Host ""
Write-Host "=== 检测结果 ===" -ForegroundColor Cyan
Write-Host "Visual Studio路径: $vsPath" -ForegroundColor White
Write-Host "MSVC版本: $latestVersion" -ForegroundColor White
Write-Host "编译器路径: $compilerPath" -ForegroundColor White
Write-Host ""

# 检查VSCode配置
$cppPropsPath = ".vscode\c_cpp_properties.json"
if (Test-Path $cppPropsPath) {
    $content = Get-Content $cppPropsPath -Raw
    if ($content -like "*$compilerPath*") {
        Write-Host "✅ VSCode配置已正确设置" -ForegroundColor Green
    } else {
        Write-Host "⚠️  VSCode配置需要更新" -ForegroundColor Yellow
        Write-Host "当前配置的编译器路径与检测到的路径不匹配" -ForegroundColor Yellow
    }
} else {
    Write-Host "⚠️  未找到VSCode C++配置文件" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "🎉 检测完成！" -ForegroundColor Green
