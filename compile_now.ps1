# OneDay Framework Immediate Compilation Script
# Simple and reliable build script with error handling

param(
    [switch]$Clean = $false,
    [switch]$Verbose = $false
)

$ErrorActionPreference = "Stop"
$StartTime = Get-Date

Write-Host "=== OneDay Framework Compilation ===" -ForegroundColor Cyan
Write-Host "Start Time: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" -ForegroundColor Yellow
Write-Host ""

function Write-Step {
    param([string]$Message, [string]$Color = "Yellow")
    Write-Host ">>> $Message" -ForegroundColor $Color
}

function Write-Success {
    param([string]$Message)
    Write-Host "✓ $Message" -ForegroundColor Green
}

function Write-Error {
    param([string]$Message)
    Write-Host "✗ $Message" -ForegroundColor Red
}

try {
    # Step 1: Environment Check
    Write-Step "Step 1: Checking build environment"
    
    # Check CMake
    if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
        throw "CMake not found. Please install CMake and add it to PATH."
    }
    $cmakeVersion = (& cmake --version | Select-Object -First 1).Split(' ')[2]
    Write-Success "CMake found: version $cmakeVersion"
    
    # Check vcpkg toolchain
    $vcpkgToolchain = "D:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake"
    if (-not (Test-Path $vcpkgToolchain)) {
        throw "vcpkg toolchain not found at: $vcpkgToolchain"
    }
    Write-Success "vcpkg toolchain found"
    
    # Check project files
    if (-not (Test-Path "CMakeLists.txt")) {
        throw "CMakeLists.txt not found. Please run this script from project root."
    }
    Write-Success "Project files verified"
    
    Write-Host ""
    
    # Step 2: Prepare build directory
    Write-Step "Step 2: Preparing build directory"
    
    if ($Clean -and (Test-Path "build")) {
        Write-Host "Cleaning existing build directory..." -ForegroundColor Yellow
        Remove-Item -Path "build" -Recurse -Force
        Write-Success "Build directory cleaned"
    }
    
    if (-not (Test-Path "build")) {
        New-Item -ItemType Directory -Path "build" | Out-Null
        Write-Success "Build directory created"
    } else {
        Write-Success "Build directory ready"
    }
    
    Set-Location "build"
    Write-Host ""
    
    # Step 3: CMake Configuration
    Write-Step "Step 3: CMake configuration"
    
    $configArgs = @(
        "-S", "..",
        "-B", ".",
        "-DCMAKE_TOOLCHAIN_FILE=$vcpkgToolchain",
        "-DCMAKE_BUILD_TYPE=Release"
    )
    
    if ($Verbose) {
        $configArgs += "--verbose"
    }
    
    Write-Host "Running: cmake $($configArgs -join ' ')" -ForegroundColor Gray
    
    & cmake @configArgs
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed with exit code: $LASTEXITCODE"
    }
    Write-Success "CMake configuration completed"
    Write-Host ""
    
    # Step 4: Build Project
    Write-Step "Step 4: Building project"
    
    $cores = [Environment]::ProcessorCount
    if ($cores -gt 1) { $cores = $cores - 1 }
    
    $buildArgs = @(
        "--build", ".",
        "--config", "Release",
        "--parallel", $cores.ToString()
    )
    
    if ($Verbose) {
        $buildArgs += "--verbose"
    }
    
    Write-Host "Running: cmake $($buildArgs -join ' ')" -ForegroundColor Gray
    Write-Host "Using $cores parallel jobs" -ForegroundColor Gray
    
    & cmake @buildArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code: $LASTEXITCODE"
    }
    Write-Success "Project build completed"
    Write-Host ""
    
    # Step 5: Verify build results
    Write-Step "Step 5: Verifying build results"
    
    # Find generated executables
    $exeFiles = Get-ChildItem -Recurse -Filter "*.exe" -ErrorAction SilentlyContinue
    if ($exeFiles) {
        Write-Success "Found $($exeFiles.Count) executable file(s):"
        foreach ($exe in $exeFiles | Select-Object -First 5) {
            $size = [math]::Round($exe.Length / 1KB, 1)
            Write-Host "  - $($exe.Name) (${size} KB)" -ForegroundColor White
        }
        if ($exeFiles.Count -gt 5) {
            Write-Host "  ... and $($exeFiles.Count - 5) more" -ForegroundColor Gray
        }
    }
    
    # Find generated libraries
    $libFiles = Get-ChildItem -Recurse -Filter "*.lib" -ErrorAction SilentlyContinue
    if ($libFiles) {
        Write-Success "Found $($libFiles.Count) library file(s)"
    }
    
    Write-Host ""
    
    # Step 6: Optional test run
    Write-Step "Step 6: Testing (optional)"
    
    if (Test-Path "CTestTestfile.cmake") {
        Write-Host "Running tests..." -ForegroundColor Yellow
        & ctest --output-on-failure --parallel 2
        if ($LASTEXITCODE -eq 0) {
            Write-Success "All tests passed"
        } else {
            Write-Host "⚠ Some tests failed, but build is successful" -ForegroundColor Yellow
        }
    } else {
        Write-Host "No tests configured" -ForegroundColor Gray
    }
    
    Write-Host ""
    
    # Success summary
    $elapsed = (Get-Date) - $StartTime
    Write-Host "=== BUILD SUCCESSFUL ===" -ForegroundColor Green
    Write-Host "Total time: $($elapsed.TotalMinutes.ToString('F1')) minutes" -ForegroundColor Green
    Write-Host "Build directory: $(Get-Location)" -ForegroundColor Green
    Write-Host ""
    
    Write-Host "Next steps:" -ForegroundColor Cyan
    Write-Host "  1. Run tests: ctest --output-on-failure" -ForegroundColor White
    Write-Host "  2. Install: cmake --install . --config Release" -ForegroundColor White
    Write-Host "  3. Package: cpack -C Release" -ForegroundColor White
    
} catch {
    Write-Host ""
    Write-Error "BUILD FAILED: $($_.Exception.Message)"
    Write-Host ""
    
    Write-Host "Troubleshooting steps:" -ForegroundColor Yellow
    Write-Host "  1. Check vcpkg status:" -ForegroundColor White
    Write-Host "     cd D:\dev\vcpkg && .\vcpkg.exe list" -ForegroundColor Gray
    Write-Host "  2. Update vcpkg:" -ForegroundColor White
    Write-Host "     cd D:\dev\vcpkg && git pull && .\vcpkg.exe update" -ForegroundColor Gray
    Write-Host "  3. Reinstall dependencies:" -ForegroundColor White
    Write-Host "     .\install_dependencies.ps1 -MaxPerformance" -ForegroundColor Gray
    Write-Host "  4. Clean build:" -ForegroundColor White
    Write-Host "     .\compile_now.ps1 -Clean -Verbose" -ForegroundColor Gray
    
    $elapsed = (Get-Date) - $StartTime
    Write-Host ""
    Write-Host "Failed after: $($elapsed.TotalMinutes.ToString('F1')) minutes" -ForegroundColor Red
    
    exit 1
} finally {
    # Return to project root
    if (Test-Path "..\CMakeLists.txt") {
        Set-Location ".."
    }
}

Write-Host ""
Write-Host "Compilation script completed successfully!" -ForegroundColor Green
