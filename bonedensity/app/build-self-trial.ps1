param([ValidateSet('ObserveG','DualWindow')][string]$Profile = 'ObserveG')
$ErrorActionPreference = 'Stop'

# Independent Debug experiment. Never builds over the original application.
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$qtRoot = 'D:\Qt\6.5.3\mingw_64'
$mingwRoot = 'D:\Qt\Tools\mingw1120_64'
$dual = $Profile -eq 'DualWindow'
$buildName = if ($dual) { 'self-trial-dual-window' } else { 'self-trial-observe-g' }
$targetName = if ($dual) { 'BoneDensity_DualWindowTrial.exe' } else { 'BoneDensity_SelfTrial.exe' }
$trialConfig = if ($dual) { 'CONFIG+=dual_window_a_trial' } else { 'CONFIG+=observe_before_g_trial' }
$guideName = if ($dual) { 'self-trial-dual-window.txt' } else { 'self-trial-observe-g.txt' }
$buildDir = Join-Path $projectRoot "build\$buildName"
$runtimeDir = Join-Path $buildDir 'debug'
$exePath = Join-Path $runtimeDir $targetName
$qmake = Join-Path $qtRoot 'bin\qmake.exe'
$make = Join-Path $mingwRoot 'bin\mingw32-make.exe'
$deploy = Join-Path $qtRoot 'bin\windeployqt.exe'
foreach ($tool in @($qmake, $make, $deploy)) {
    if (-not (Test-Path -LiteralPath $tool)) { throw "Required tool missing: $tool" }
}
$env:PATH = "$mingwRoot\bin;$qtRoot\bin;$env:PATH"
New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
Push-Location $buildDir
try {
    & $qmake -o Makefile (Join-Path $projectRoot 'BoneDensity.pro') -spec win32-g++ 'CONFIG-=debug_and_release' 'CONFIG-=release' 'CONFIG+=debug' $trialConfig 'DESTDIR=debug' "QMAKE_CXX=$mingwRoot/bin/g++.exe" "QMAKE_LINK=$mingwRoot/bin/g++.exe" "QMAKE_LINK_C=$mingwRoot/bin/gcc.exe" "QMAKE_CC=$mingwRoot/bin/gcc.exe"
    if ($LASTEXITCODE -ne 0) { throw "qmake failed: $LASTEXITCODE" }
    & $make -f Makefile -j4
    if ($LASTEXITCODE -ne 0) { throw "Build failed: $LASTEXITCODE" }
    & $deploy --debug --no-translations --compiler-runtime $exePath
    if ($LASTEXITCODE -ne 0) { throw "Deployment failed: $LASTEXITCODE" }
    foreach ($runtime in @('libgcc_s_seh-1.dll', 'libstdc++-6.dll', 'libwinpthread-1.dll')) {
        Copy-Item -LiteralPath (Join-Path $mingwRoot "bin\$runtime") -Destination $runtimeDir -Force
    }
    Copy-Item -LiteralPath (Join-Path $projectRoot "docs\guides\$guideName") -Destination (Join-Path $runtimeDir 'README.txt') -Force
} finally {
    Pop-Location
}
Write-Host "Independent self-trial build: $exePath"
Write-Host 'Original application and patient files were not copied or changed.'
