$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$qtRoot = "D:\Qt\6.5.3\mingw_64"
$mingwRoot = "D:\Qt\Tools\mingw1120_64"
$buildDir = Join-Path $projectRoot "build\debug"
$exePath = Join-Path $buildDir "debug\BoneDensity.exe"
$portableAssets = Join-Path $projectRoot "portable"

$qmake = Join-Path $qtRoot "bin\qmake.exe"
$make = Join-Path $mingwRoot "bin\mingw32-make.exe"
$deploy = Join-Path $qtRoot "bin\windeployqt.exe"

foreach ($tool in @($qmake, $make, $deploy)) {
    if (-not (Test-Path -LiteralPath $tool)) {
        throw "Required Qt tool was not found: $tool"
    }
}

$env:PATH = "$mingwRoot\bin;$qtRoot\bin;$env:PATH"
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
Push-Location $buildDir
try {
    & $qmake -o Makefile "..\..\BoneDensity.pro" -spec win32-g++ "CONFIG+=debug"
    if ($LASTEXITCODE -ne 0) { throw "qmake failed with exit code $LASTEXITCODE" }

    & $make clean
    if ($LASTEXITCODE -ne 0) { throw "clean failed with exit code $LASTEXITCODE" }

    & $make -j4
    if ($LASTEXITCODE -ne 0) { throw "build failed with exit code $LASTEXITCODE" }

    & $deploy --debug --no-translations --compiler-runtime $exePath
    if ($LASTEXITCODE -ne 0) { throw "windeployqt failed with exit code $LASTEXITCODE" }

    foreach ($runtime in @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")) {
        Copy-Item -LiteralPath (Join-Path $mingwRoot "bin\$runtime") `
                  -Destination (Join-Path (Split-Path -Parent $exePath) $runtime) -Force
    }

    if (-not (Test-Path -LiteralPath $portableAssets -PathType Container)) {
        throw "Portable handoff assets were not found: $portableAssets"
    }
    Get-ChildItem -LiteralPath $portableAssets -File | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination (Split-Path -Parent $exePath) -Force
    }
    $handoffImplementation = Get-ChildItem -LiteralPath (Split-Path -Parent $exePath) -File -Filter '*.ps1' |
        Select-Object -First 1
    if ($null -ne $handoffImplementation) {
        $handoffImplementation.Attributes = $handoffImplementation.Attributes -bor [System.IO.FileAttributes]::Hidden
    }
} finally {
    Pop-Location
}

Write-Host "Debug build completed: $exePath"
