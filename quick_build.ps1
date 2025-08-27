# OneDay Framework 快速编译脚本
# 简化版编译流程

param(
    [switch]$Clean = $false,
    [switch]$Test = $false
)

$ErrorActionPreference = "Stop"
$StartTime = Get-Date

Write-Host "🚀 OneDay Framework 快速编译" -ForegroundColor Cyan
Write-Host ""

try {
    # 检查基本环境
    Write-Host "检查环境..." -ForegroundColor Yellow
    
    if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
        throw "未找到CMake，请先安装CMake"
    }
    
    if (-not (Test-Path "CMakeLists.txt")) {
        throw "未找到CMakeLists.txt，请在项目根目录运行此脚本"
    }
    
    if (-not (Test-Path "D:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake")) {
        throw "未找到vcpkg工具链，请检查vcpkg安装"
    }
    
    Write-Host "✅ 环境检查通过" -ForegroundColor Green
    
    # 清理构建目录
    if ($Clean -and (Test-Path "build")) {
        Write-Host "清理构建目录..." -ForegroundColor Yellow
        Remove-Item -Path "build" -Recurse -Force
        Write-Host "✅ 构建目录已清理" -ForegroundColor Green
    }
    
    # 创建构建目录
    Write-Host "准备构建目录..." -ForegroundColor Yellow
    if (-not (Test-Path "build")) {
        New-Item -ItemType Directory -Path "build" | Out-Null
    }
    
    Set-Location "build"
    Write-Host "✅ 构建目录已准备" -ForegroundColor Green
    
    # CMake配置
    Write-Host "CMake配置..." -ForegroundColor Yellow
    $configArgs = @(
        "-S", "..",
        "-B", ".",
        "-DCMAKE_TOOLCHAIN_FILE=D:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake",
        "-DCMAKE_BUILD_TYPE=Release"
    )
    
    & cmake @configArgs
    if ($LASTEXITCODE -ne 0) {
        throw "CMake配置失败"
    }
    Write-Host "✅ CMake配置成功" -ForegroundColor Green
    
    # 编译项目
    Write-Host "编译项目..." -ForegroundColor Yellow
    $cores = [Environment]::ProcessorCount - 1
    if ($cores -lt 1) { $cores = 1 }
    
    & cmake --build . --config Release --parallel $cores
    if ($LASTEXITCODE -ne 0) {
        throw "编译失败"
    }
    Write-Host "✅ 编译成功" -ForegroundColor Green
    
    # 运行测试
    if ($Test) {
        Write-Host "运行测试..." -ForegroundColor Yellow
        & ctest --output-on-failure --parallel $cores
        if ($LASTEXITCODE -ne 0) {
            Write-Host "⚠️  测试失败，但编译成功" -ForegroundColor Yellow
        } else {
            Write-Host "✅ 测试通过" -ForegroundColor Green
        }
    }
    
    # 显示结果
    Write-Host ""
    Write-Host "🎉 编译完成！" -ForegroundColor Green
    
    $elapsed = (Get-Date) - $StartTime
    Write-Host "耗时: $($elapsed.TotalMinutes.ToString('F1')) 分钟" -ForegroundColor Cyan
    
    # 查找生成的文件
    $exeFiles = Get-ChildItem -Recurse -Filter "*.exe" | Select-Object -First 3
    if ($exeFiles) {
        Write-Host ""
        Write-Host "生成的可执行文件:" -ForegroundColor Cyan
        foreach ($exe in $exeFiles) {
            Write-Host "  📁 $($exe.FullName)" -ForegroundColor White
        }
    }
    
} catch {
    Write-Host ""
    Write-Host "❌ 编译失败: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host ""
    Write-Host "故障排除建议:" -ForegroundColor Yellow
    Write-Host "  1. 检查vcpkg是否正确安装: D:\dev\vcpkg\vcpkg.exe list" -ForegroundColor White
    Write-Host "  2. 更新vcpkg: cd D:\dev\vcpkg && git pull && .\vcpkg.exe update" -ForegroundColor White
    Write-Host "  3. 重新安装依赖: .\install_dependencies.ps1 -MaxPerformance" -ForegroundColor White
    Write-Host "  4. 使用详细编译脚本: .\build_project.ps1 -Verbose" -ForegroundColor White
    
    exit 1
} finally {
    # 返回原始目录
    if (Test-Path "..\CMakeLists.txt") {
        Set-Location ".."
    }
}

Write-Host ""
Write-Host "使用方法:" -ForegroundColor Cyan
Write-Host "  .\quick_build.ps1           # 快速编译" -ForegroundColor White
Write-Host "  .\quick_build.ps1 -Clean    # 清理后编译" -ForegroundColor White
Write-Host "  .\quick_build.ps1 -Test     # 编译并运行测试" -ForegroundColor White
