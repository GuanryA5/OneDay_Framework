# OneDay Framework Environment Check Script
# Run with Administrator privileges in PowerShell

param(
    [switch]$Detailed
)

Write-Host "OneDay Framework Environment Check" -ForegroundColor Cyan
Write-Host "=================================" -ForegroundColor Cyan
Write-Host ""

$totalChecks = 7
$passedChecks = 0

# Helper functions
function Test-Command {
    param($CommandName)
    $null = Get-Command $CommandName -ErrorAction SilentlyContinue
    return $?
}

function Write-CheckResult {
    param(
        [string]$TestName,
        [bool]$Passed,
        [string]$Details = ""
    )
    
    if ($Passed) {
        Write-Host "✅ $TestName" -ForegroundColor Green
        if ($Details) { Write-Host "   $Details" -ForegroundColor Gray }
        return 1
    }
    else {
        Write-Host "❌ $TestName" -ForegroundColor Red  
        if ($Details) { Write-Host "   $Details" -ForegroundColor Yellow }
        return 0
    }
}

# [1/7] Check Visual Studio MSVC Compiler
Write-Host "[1/7] Checking Visual Studio MSVC Compiler..." -ForegroundColor Blue
$clExists = Test-Command "cl"
if ($clExists) {
    try {
        $clOutput = & cl 2>&1 | Out-String
        $clVersion = ($clOutput -split "`n" | Where-Object { $_ -match "Version|版本" })[0]
        if ($clVersion) {
            $passedChecks += Write-CheckResult "MSVC Compiler Installed" $true $clVersion.Trim()
        }
        else {
            $passedChecks += Write-CheckResult "MSVC Compiler Installed" $true "Found but version info unavailable"
        }
    }
    catch {
        $passedChecks += Write-CheckResult "MSVC Compiler Installed" $true "Found but version check failed"
    }
}
else {
    $passedChecks += Write-CheckResult "MSVC Compiler Not Found" $false "Please install Visual Studio 2022 and add to PATH"
}

# [2/7] Check CMake Version
Write-Host "`n[2/7] Checking CMake Version..." -ForegroundColor Blue
$cmakeExists = Test-Command "cmake"
if ($cmakeExists) {
    try {
        $cmakeOutput = & cmake --version 2>&1 | Out-String
        if ($cmakeOutput -match "cmake version (\d+\.\d+\.\d+)") {
            $cmakeVersion = $matches[1]
            $versionParts = $cmakeVersion.Split('.')
            $majorVersion = [int]$versionParts[0]
            $minorVersion = [int]$versionParts[1]
            
            $versionOk = ($majorVersion -gt 3) -or ($majorVersion -eq 3 -and $minorVersion -ge 25)
            $details = "Current version: $cmakeVersion"
            if (-not $versionOk) { $details += " (Requires 3.25+)" }
            $passedChecks += Write-CheckResult "CMake Version Check" $versionOk $details
        }
        else {
            $passedChecks += Write-CheckResult "CMake Version Check" $false "Cannot get version info"
        }
    }
    catch {
        $passedChecks += Write-CheckResult "CMake Version Check" $false "Cannot execute cmake command"
    }
}
else {
    $passedChecks += Write-CheckResult "CMake Not Found" $false "Please install CMake 3.25+"
}

# [3/7] Check vcpkg Installation
Write-Host "`n[3/7] Checking vcpkg Installation..." -ForegroundColor Blue
$vcpkgPath = "D:\dev\vcpkg\vcpkg.exe"
if (Test-Path $vcpkgPath) {
    try {
        $vcpkgOutput = & $vcpkgPath version 2>&1 | Out-String
        $vcpkgVersion = ($vcpkgOutput -split "`n")[0].Trim()
        $passedChecks += Write-CheckResult "vcpkg Installed" $true $vcpkgVersion
    }
    catch {
        $passedChecks += Write-CheckResult "vcpkg Installed" $true "Installed at C:\vcpkg"
    }
}
else {
    $passedChecks += Write-CheckResult "vcpkg Not Found" $false "Please install to D:\dev\vcpkg"
}

# [4/7] Check Qt Package
Write-Host "`n[4/7] Checking Qt Package..." -ForegroundColor Blue
if (Test-Path $vcpkgPath) {
    try {
        $vcpkgList = & $vcpkgPath list 2>&1 | Out-String
        $qtInstalled = $vcpkgList -match "qtbase"
        if ($qtInstalled) {
            $passedChecks += Write-CheckResult "Qt Package Check" $true "Qt is installed"
        }
        else {
            $passedChecks += Write-CheckResult "Qt Package Check" $false "Run: .\install_dependencies.ps1"
        }
    }
    catch {
        $passedChecks += Write-CheckResult "Qt Package Check" $false "Cannot check vcpkg package status"
    }
}
else {
    Write-Host "   Skipping Qt check (vcpkg not installed)" -ForegroundColor Yellow
}

# [5/7] Check OpenCV Package  
Write-Host "`n[5/7] Checking OpenCV Package..." -ForegroundColor Blue
if (Test-Path $vcpkgPath) {
    try {
        $vcpkgList = & $vcpkgPath list 2>&1 | Out-String
        $opencvInstalled = $vcpkgList -match "opencv4"
        if ($opencvInstalled) {
            $passedChecks += Write-CheckResult "OpenCV Package Check" $true "OpenCV is installed"
        }
        else {
            $passedChecks += Write-CheckResult "OpenCV Package Check" $false "Run: .\install_dependencies.ps1"
        }
    }
    catch {
        $passedChecks += Write-CheckResult "OpenCV Package Check" $false "Cannot check vcpkg package status"
    }
}
else {
    Write-Host "   Skipping OpenCV check (vcpkg not installed)" -ForegroundColor Yellow
}

# [6/7] Check Other Dependencies (based on vcpkg.json)
Write-Host "`n[6/7] Checking Other Dependencies..." -ForegroundColor Blue
if (Test-Path $vcpkgPath) {
    # Dependencies from vcpkg.json (excluding qtbase, qttools, opencv4 already checked)
    $dependencies = @("onnx", "gtest", "boost-geometry", "eigen3", "nlohmann-json", "spdlog")
    $missingDeps = @()

    try {
        $vcpkgList = & $vcpkgPath list 2>&1 | Out-String
        foreach ($dep in $dependencies) {
            $depInstalled = $vcpkgList -match $dep
            if (-not $depInstalled) {
                $missingDeps += $dep
            }
        }

        $allInstalled = $missingDeps.Count -eq 0
        if ($allInstalled) {
            $passedChecks += Write-CheckResult "Other Dependencies" $true "All dependencies installed"
        }
        else {
            $missingList = $missingDeps -join ", "
            $passedChecks += Write-CheckResult "Other Dependencies" $false "Missing: $missingList"
        }
    }
    catch {
        $passedChecks += Write-CheckResult "Other Dependencies" $false "Cannot check vcpkg package status"
    }
}
else {
    Write-Host "   Skipping dependencies check (vcpkg not installed)" -ForegroundColor Yellow
}

# [7/7] Check Project Structure
Write-Host "`n[7/7] Checking Project Structure..." -ForegroundColor Blue
$projectFiles = @("CMakeLists.txt", "vcpkg.json", "src\main.cpp", "SETUP_GUIDE.md")
$missingFiles = @()

foreach ($file in $projectFiles) {
    if (-not (Test-Path $file)) {
        $missingFiles += $file
    }
}

$projectOk = $missingFiles.Count -eq 0
if ($projectOk) {
    $passedChecks += Write-CheckResult "Project Structure Complete" $true
}
else {
    $missingList = $missingFiles -join ", "
    $passedChecks += Write-CheckResult "Project Structure Check" $false "Missing files: $missingList"
}

# Summary
Write-Host "`n=================================" -ForegroundColor Cyan
Write-Host "Environment Check Complete" -ForegroundColor Cyan
Write-Host "Passed Checks: $passedChecks/$totalChecks" -ForegroundColor $(if ($passedChecks -eq $totalChecks) { "Green" } else { "Yellow" })

if ($passedChecks -eq $totalChecks) {
    Write-Host "`n🎉 All checks passed! Ready to build:" -ForegroundColor Green
    Write-Host ""
    Write-Host "Build Commands:" -ForegroundColor Cyan
    Write-Host "  mkdir build" -ForegroundColor White
    Write-Host "  cd build" -ForegroundColor White  
    Write-Host "  cmake -S .. -B . -DCMAKE_TOOLCHAIN_FILE=D:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake" -ForegroundColor White
    Write-Host "  cmake --build . --config Release" -ForegroundColor White
}
else {
    Write-Host "`n⚠️  Need to fix $($totalChecks - $passedChecks) issues before building" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "See detailed installation guide: SETUP_GUIDE.md" -ForegroundColor Cyan
}

Write-Host ""
Read-Host "Press Enter to exit"