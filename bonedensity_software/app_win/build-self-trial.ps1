param(
    [ValidateSet('ObserveG','DualWindow')][string]$Profile = 'ObserveG',
    [string]$qtRoot = 'D:\QT6.5.3\6.5.3\mingw_64',
    [string]$mingwRoot = 'D:\QT6.5.3\Tools\mingw1120_64'
)
$ErrorActionPreference = 'Stop'

# Independent Debug experiment. Never builds over the original application.
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
. (Join-Path $projectRoot 'scripts\toolchain.ps1')
$tools = Initialize-QtToolchain -QtRoot $qtRoot -MingwRoot $mingwRoot -Deploy

$dual = $Profile -eq 'DualWindow'
$buildName = if ($dual) { 'self-trial-dual-window' } else { 'self-trial-observe-g' }
$targetName = if ($dual) { 'BoneDensity_DualWindowTrial.exe' } else { 'BoneDensity_SelfTrial.exe' }
$trialConfig = if ($dual) { 'CONFIG+=dual_window_a_trial' } else { 'CONFIG+=observe_before_g_trial' }
$guideName = if ($dual) { 'self-trial-dual-window.txt' } else { 'self-trial-observe-g.txt' }
$buildDir = Join-Path $projectRoot "build\$buildName"
$runtimeDir = Join-Path $buildDir 'debug'
$exePath = Join-Path $runtimeDir $targetName

Invoke-QMake -Tools $tools -Project (Join-Path $projectRoot 'BoneDensity.pro') -BuildDir $buildDir `
             -Arguments @('CONFIG-=debug_and_release', 'CONFIG-=release', 'CONFIG+=debug', $trialConfig,
                          'DESTDIR=debug', "QMAKE_CXX=$mingwRoot/bin/g++.exe", "QMAKE_LINK=$mingwRoot/bin/g++.exe",
                          "QMAKE_LINK_C=$mingwRoot/bin/gcc.exe", "QMAKE_CC=$mingwRoot/bin/gcc.exe")
Invoke-Make -Tools $tools -BuildDir $buildDir

& $tools.deploy --debug --no-translations --compiler-runtime $exePath
if ($LASTEXITCODE -ne 0) { throw "Deployment failed: $LASTEXITCODE" }
Copy-MingwRuntime -Tools $tools -Destination $runtimeDir
Copy-Item -LiteralPath (Join-Path $projectRoot "docs\guides\$guideName") -Destination (Join-Path $runtimeDir 'README.txt') -Force

Write-Host "Independent self-trial build: $exePath"
Write-Host 'Original application and patient files were not copied or changed.'
