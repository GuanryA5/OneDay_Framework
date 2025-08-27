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

# 手动检测Visual Studio路径
function Find-VisualStudioPathManual {
    $possiblePaths = @(
        "C:\Program Files\Microsoft Visual Studio\2022\Professional",
        "C:\Program Files\Microsoft Visual Studio\2022\Community",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise",
        "D:\Applications\Microsoft Visual Studio\2022\Professional",
        "D:\Applications\Microsoft Visual Studio\2022\Community",
        "D:\Applications\Microsoft Visual Studio\2022\Enterprise"
    )
    
    foreach ($path in $possiblePaths) {
        $vcToolsPath = Join-Path $path "VC\Tools\MSVC"
        if (Test-Path $vcToolsPath) {
            Write-Success "✅ 手动找到Visual Studio: $path"
            return $path
        }
    }
    
    Write-Error "❌ 未找到任何Visual Studio 2022安装"
    return $null
}

# 获取MSVC编译器路径
function Get-MSVCCompilerPath($vsPath) {
    $vcToolsPath = Join-Path $vsPath "VC\Tools\MSVC"
    
    if (-not (Test-Path $vcToolsPath)) {
        Write-Error "❌ 未找到VC Tools目录: $vcToolsPath"
        return $null
    }
    
    # 找到最新版本的MSVC
    $msvcVersions = Get-ChildItem $vcToolsPath | Where-Object { $_.PSIsContainer } | Sort-Object Name -Descending
    
    if (-not $msvcVersions) {
        Write-Error "❌ 未找到MSVC版本目录"
        return $null
    }
    
    $latestVersion = $msvcVersions[0].Name
    $compilerPath = Join-Path $vcToolsPath "$latestVersion\bin\Hostx64\x64\cl.exe"
    
    if (Test-Path $compilerPath) {
        Write-Success "✅ 找到编译器: $compilerPath"
        return @{
            CompilerPath = $compilerPath
            MSVCVersion  = $latestVersion
            VSPath       = $vsPath
        }
    }
    
    Write-Error "❌ 未找到编译器: $compilerPath"
    return $null
}

# 更新VSCode配置文件
function Update-VSCodeConfig($compilerInfo) {
    $vscodePath = ".vscode"
    
    if (-not (Test-Path $vscodePath)) {
        Write-Warning "⚠️  未找到.vscode目录，跳过配置更新"
        return
    }
    
    # 更新c_cpp_properties.json
    $cppPropsPath = Join-Path $vscodePath "c_cpp_properties.json"
    if (Test-Path $cppPropsPath) {
        try {
            $cppProps = Get-Content $cppPropsPath -Raw | ConvertFrom-Json
            
            # 更新编译器路径
            $cppProps.configurations[0].compilerPath = $compilerInfo.CompilerPath
            
            # 更新包含路径
            $vcIncludePath = "$($compilerInfo.VSPath)\VC\Tools\MSVC\*\include\**"
            $cppProps.configurations[0].includePath = $cppProps.configurations[0].includePath | ForEach-Object {
                if ($_ -like "*Microsoft Visual Studio*VC\Tools\MSVC*") {
                    $vcIncludePath
                }
                else {
                    $_
                }
            }
            
            $cppProps | ConvertTo-Json -Depth 10 | Set-Content $cppPropsPath -Encoding UTF8
            Write-Success "✅ 已更新 c_cpp_properties.json"
        }
        catch {
            Write-Error "❌ 更新c_cpp_properties.json失败: $($_.Exception.Message)"
        }
    }
    
    # 更新settings.json
    $settingsPath = Join-Path $vscodePath "settings.json"
    if (Test-Path $settingsPath) {
        try {
            $settings = Get-Content $settingsPath -Raw | ConvertFrom-Json
            
            # 更新开发者命令提示符路径
            $devCmdPath = "$($compilerInfo.VSPath)\Common7\Tools\VsDevCmd.bat"
            if ($settings.'terminal.integrated.profiles.windows'.'Developer Command Prompt') {
                $settings.'terminal.integrated.profiles.windows'.'Developer Command Prompt'.args[1] = $devCmdPath
            }
            
            $settings | ConvertTo-Json -Depth 10 | Set-Content $settingsPath -Encoding UTF8
            Write-Success "✅ 已更新 settings.json"
        }
        catch {
            Write-Error "❌ 更新settings.json失败: $($_.Exception.Message)"
        }
    }
}

# 主执行逻辑
try {
    # 检测Visual Studio路径
    $vsPath = Find-VisualStudioPath
    
    if (-not $vsPath) {
        Write-Error "❌ 无法找到Visual Studio 2022安装"
        exit 1
    }
    
    # 获取编译器信息
    $compilerInfo = Get-MSVCCompilerPath $vsPath
    
    if (-not $compilerInfo) {
        Write-Error "❌ 无法找到MSVC编译器"
        exit 1
    }
    
    # 显示检测结果
    Write-Info ""
    Write-Info "=== 检测结果 ==="
    Write-Info "Visual Studio路径: $($compilerInfo.VSPath)"
    Write-Info "MSVC版本: $($compilerInfo.MSVCVersion)"
    Write-Info "编译器路径: $($compilerInfo.CompilerPath)"
    Write-Info ""
    
    # 更新配置文件
    if ($UpdateConfig) {
        Write-Info "=== 更新VSCode配置 ==="
        Update-VSCodeConfig $compilerInfo
        Write-Success "✅ 配置更新完成！"
    }
    else {
        Write-Info "💡 要自动更新VSCode配置，请运行："
        Write-Info "   .\scripts\detect_vs_path.ps1 -UpdateConfig"
    }
    
    Write-Info ""
    Write-Success "🎉 检测完成！"
    
}
catch {
    Write-Error "❌ 脚本执行失败: $($_.Exception.Message)"
    if ($Verbose) {
        Write-Error $_.Exception.StackTrace
    }
    exit 1
}
