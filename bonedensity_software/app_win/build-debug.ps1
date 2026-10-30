param(
    [string]$qtRoot = "D:\QT6.5.3\6.5.3\mingw_64",
    [string]$mingwRoot = "D:\QT6.5.3\Tools\mingw1120_64"
)
$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
. (Join-Path $projectRoot "scripts\toolchain.ps1")
$tools = Initialize-QtToolchain -QtRoot $qtRoot -MingwRoot $mingwRoot -Deploy

$buildDir = Join-Path $projectRoot "build\debug"
$exePath = Join-Path $buildDir "debug\BoneDensity.exe"
$outputDir = Split-Path -Parent $exePath

Invoke-QMake -Tools $tools -Project (Join-Path $projectRoot "BoneDensity.pro") -BuildDir $buildDir `
             -Arguments @("CONFIG+=debug")
Invoke-Make -Tools $tools -BuildDir $buildDir -Clean

& $tools.deploy --debug --no-translations --compiler-runtime $exePath
if ($LASTEXITCODE -ne 0) { throw "windeployqt failed with exit code $LASTEXITCODE" }
Copy-MingwRuntime -Tools $tools -Destination $outputDir
Copy-PortableAssets -ProjectRoot $projectRoot -Destination $outputDir

Write-Host "Debug build completed: $exePath"
