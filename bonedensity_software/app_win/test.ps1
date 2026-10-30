# Builds and runs the automated test suites (all by default).
#   core          device protocol and measurement core, Qt Core only
#   accountstore  account store
#   patientstore  patient/measurement store
#   calibration   calibration calculation, store and wizard
#   mainwindow    main-window regression suite (offscreen)
#   handoff       portable hand-off scripts
# Example: powershell -ExecutionPolicy Bypass -File ./test.ps1 -Suite core,mainwindow -Incremental
param(
    [ValidateSet('core', 'accountstore', 'patientstore', 'calibration', 'mainwindow', 'handoff')]
    [string[]]$Suite = @('core', 'accountstore', 'patientstore', 'calibration', 'mainwindow', 'handoff'),
    # Reuse the previous build instead of building from clean.
    [switch]$Incremental,
    [string]$qtRoot = "D:\QT6.5.3\6.5.3\mingw_64",
    [string]$mingwRoot = "D:\QT6.5.3\Tools\mingw1120_64"
)
$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
. (Join-Path $projectRoot "scripts\toolchain.ps1")
$tools = Initialize-QtToolchain -QtRoot $qtRoot -MingwRoot $mingwRoot
$env:QT_QPA_PLATFORM = "offscreen"
# The offscreen platform renders no text without a font directory.
if (-not $env:QT_QPA_FONTDIR) { $env:QT_QPA_FONTDIR = Join-Path $env:WINDIR "Fonts" }

$targets = @{
    core = 'core_tests'
    accountstore = 'accountstore_tests'
    patientstore = 'patientstore_tests'
    calibration = 'calibration_tests'
    mainwindow = 'mainwindow_safety_tests'
}
$failed = @()
foreach ($name in $Suite) {
    Write-Host "== $name"
    try {
        if ($name -eq 'handoff') {
            & powershell -ExecutionPolicy Bypass -File (Join-Path $projectRoot "tests\portable_handoff_tests.ps1")
            if ($LASTEXITCODE -ne 0) { throw "exit code $LASTEXITCODE" }
            continue
        }
        $target = $targets[$name]
        $buildDir = Join-Path $projectRoot "build\tests\$name"
        Invoke-QMake -Tools $tools -Project (Join-Path $projectRoot "tests\$target.pro") -BuildDir $buildDir `
                     -Arguments @("CONFIG+=debug")
        Invoke-Make -Tools $tools -BuildDir $buildDir -Clean:(-not $Incremental)
        & (Join-Path $buildDir "debug\$target.exe") -txt
        if ($LASTEXITCODE -ne 0) { throw "$LASTEXITCODE failing test(s)" }
    } catch {
        $failed += "$name ($_)"
        Write-Host "FAILED: $name - $_" -ForegroundColor Red
    }
}

if ($failed.Count -gt 0) { throw "Test suites failed: $($failed -join '; ')" }
Write-Host "All selected test suites passed: $($Suite -join ', ')"
