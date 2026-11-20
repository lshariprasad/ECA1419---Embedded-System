$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$SourceDir = $PSScriptRoot
$BuildDir = Join-Path $SourceDir "build"
$OutputDir = Join-Path $ProjectRoot "output"
$LogFile = Join-Path $OutputDir "systemc_tlm_run.txt"
$ToolsDir = "C:\msys64\ucrt64\bin"

New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
if (-not (Test-Path (Join-Path $ToolsDir "g++.exe"))) {
    throw "MSYS2 UCRT64 g++.exe not found at $ToolsDir. Install the UCRT64 GCC toolchain first."
}
if (-not (Test-Path (Join-Path $ToolsDir "cmake.exe"))) {
    throw "CMake not found in $ToolsDir. Install mingw-w64-ucrt-x86_64-cmake."
}
if (-not (Test-Path (Join-Path $ToolsDir "ninja.exe"))) {
    throw "Ninja not found in $ToolsDir. Install mingw-w64-ucrt-x86_64-ninja."
}
if (-not (Test-Path (Join-Path $ToolsDir "libsystemc-3.0.2.dll"))) {
    Write-Warning "SystemC runtime DLL not found under $ToolsDir. Check the installed SystemC package/version."
}

$env:Path = "$ToolsDir;$env:Path"
Write-Host "[1/3] Configuring SystemC TLM project..." -ForegroundColor Cyan
cmake -S $SourceDir -B $BuildDir -G Ninja `
  -DCMAKE_MAKE_PROGRAM="$ToolsDir/ninja.exe" `
  -DCMAKE_CXX_COMPILER="$ToolsDir/g++.exe" `
  -DCMAKE_PREFIX_PATH="$ToolsDir/.."

Write-Host "[2/3] Building..." -ForegroundColor Cyan
cmake --build $BuildDir

Write-Host "[3/3] Running TLM production-line simulation..." -ForegroundColor Cyan
$Exe = Join-Path $BuildDir "exp09_tlm.exe"
if (-not (Test-Path $Exe)) { throw "Expected executable was not created: $Exe" }
& $Exe 2>&1 | Tee-Object -FilePath $LogFile
if ($LASTEXITCODE -ne 0) { throw "SystemC simulation returned exit code $LASTEXITCODE" }
Write-Host "Run log saved to: $LogFile" -ForegroundColor Green
