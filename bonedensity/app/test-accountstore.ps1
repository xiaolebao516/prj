$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$qtRoot = "D:\Qt\6.5.3\mingw_64"
$mingwRoot = "D:\Qt\Tools\mingw1120_64"
$buildDir = Join-Path $projectRoot "build\tests\accountstore"
$testExe = Join-Path $buildDir "debug\accountstore_tests.exe"

$qmake = Join-Path $qtRoot "bin\qmake.exe"
$make = Join-Path $mingwRoot "bin\mingw32-make.exe"

foreach ($tool in @($qmake, $make)) {
    if (-not (Test-Path -LiteralPath $tool)) {
        throw "Required Qt tool was not found: $tool"
    }
}

$env:PATH = "$mingwRoot\bin;$qtRoot\bin;$env:PATH"
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
Push-Location $buildDir
try {
    & $qmake -o Makefile "..\..\..\tests\accountstore_tests.pro" -spec win32-g++ "CONFIG+=debug"
    if ($LASTEXITCODE -ne 0) { throw "qmake failed with exit code $LASTEXITCODE" }

    & $make clean
    if ($LASTEXITCODE -ne 0) { throw "clean failed with exit code $LASTEXITCODE" }

    & $make -j4
    if ($LASTEXITCODE -ne 0) { throw "build failed with exit code $LASTEXITCODE" }

    & $testExe -txt
    if ($LASTEXITCODE -ne 0) { throw "account store tests failed with exit code $LASTEXITCODE" }
} finally {
    Pop-Location
}

Write-Host "Account store tests passed: $testExe"
