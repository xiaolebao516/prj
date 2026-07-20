$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$projectRoot = Split-Path -Parent $PSScriptRoot
$assetRoot = Join-Path $projectRoot 'portable'
$handoffScriptName = '一键换机备份.ps1'
$testRoot = Join-Path ([System.IO.Path]::GetTempPath()) ('BoneDensityPortableTests-' + [guid]::NewGuid().ToString('N'))

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw "Assertion failed: $Message" }
}

function New-Fixture([string]$Name, [switch]$IncludeOptionalData) {
    $root = Join-Path $testRoot $Name
    New-Item -ItemType Directory -Path $root -Force | Out-Null
    foreach ($asset in @('一键换机备份.cmd', '一键换机备份.ps1', '换机说明.txt')) {
        Copy-Item -LiteralPath (Join-Path $assetRoot $asset) -Destination $root
    }
    foreach ($file in @(
        'BoneDensity.exe', 'Qt6Core.dll', 'Qt6Gui.dll', 'Qt6Widgets.dll', 'Qt6Xml.dll',
        'Qt6SerialPort.dll', 'Qt6PrintSupport.dll', 'libgcc_s_seh-1.dll',
        'libstdc++-6.dll', 'libwinpthread-1.dll', 'accounts.xml', 'calibration.xml')) {
        Set-Content -LiteralPath (Join-Path $root $file) -Value "fixture:$file" -Encoding utf8
    }
    $platforms = Join-Path $root 'platforms'
    New-Item -ItemType Directory -Path $platforms -Force | Out-Null
    Set-Content -LiteralPath (Join-Path $platforms 'qwindows.dll') -Value 'fixture:platform' -Encoding utf8
    if ($IncludeOptionalData) {
        Set-Content -LiteralPath (Join-Path $root 'patients.xml') -Value '<patients version="2" />' -Encoding utf8
        Set-Content -LiteralPath (Join-Path $root 'measurements.xml') -Value '<measurements version="1" />' -Encoding utf8
    }
    return $root
}

function Invoke-Handoff([string]$Source, [string]$Target) {
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File (Join-Path $Source $handoffScriptName) `
        -TargetRoot $Target -NonInteractive
    if ($LASTEXITCODE -ne 0) { throw "Handoff process failed with exit code $LASTEXITCODE" }
}

function Get-Packages([string]$Target) {
    return @(Get-ChildItem -LiteralPath $Target -Directory |
        Where-Object { $_.Name -like 'BoneDensity-换机包-*' -and -not $_.Name.EndsWith('.partial') })
}

try {
    New-Item -ItemType Directory -Path $testRoot -Force | Out-Null

    $successSource = New-Fixture 'success' -IncludeOptionalData
    $successTarget = Join-Path $testRoot 'target'
    New-Item -ItemType Directory -Path $successTarget | Out-Null
    Invoke-Handoff $successSource $successTarget
    $firstPackage = @(Get-Packages $successTarget)
    Assert-True ($firstPackage.Count -eq 1) 'first package should be created'
    $packagePath = $firstPackage[0].FullName
    foreach ($file in @('BoneDensity.exe', 'accounts.xml', 'calibration.xml', 'patients.xml', 'measurements.xml', '换机说明.txt', '换机包清单.txt')) {
        Assert-True (Test-Path -LiteralPath (Join-Path $packagePath $file) -PathType Leaf) "package should contain $file"
    }
    $internalScript = Get-ChildItem -LiteralPath $packagePath -Force -File -Filter '*.ps1' | Select-Object -First 1
    Assert-True ($null -ne $internalScript) 'package should contain its internal handoff script'
    Assert-True (($internalScript.Attributes -band [System.IO.FileAttributes]::Hidden) -ne 0) 'internal handoff script should be hidden'
    $sourceHash = (Get-FileHash -LiteralPath (Join-Path $successSource 'accounts.xml') -Algorithm SHA256).Hash
    $packageHash = (Get-FileHash -LiteralPath (Join-Path $packagePath 'accounts.xml') -Algorithm SHA256).Hash
    Assert-True ($sourceHash -eq $packageHash) 'accounts.xml hash should be preserved'
    Assert-True ((Get-Content -LiteralPath (Join-Path $packagePath '换机包清单.txt') -Raw) -match 'accounts.xml SHA256') 'manifest should list account hash'

    Invoke-Handoff $successSource $successTarget
    Assert-True (@(Get-Packages $successTarget).Count -eq 2) 'second package should not overwrite the first package'

    $noHistorySource = New-Fixture 'no-history'
    $noHistoryTarget = Join-Path $testRoot 'no-history-target'
    New-Item -ItemType Directory -Path $noHistoryTarget | Out-Null
    Invoke-Handoff $noHistorySource $noHistoryTarget
    $noHistoryPackage = (@(Get-Packages $noHistoryTarget))[0].FullName
    Assert-True (-not (Test-Path -LiteralPath (Join-Path $noHistoryPackage 'patients.xml'))) 'missing optional patient data should remain absent'
    Assert-True (Test-Path -LiteralPath (Join-Path $noHistoryPackage 'accounts.xml')) 'required account data should remain present'

    $missingRuntimeSource = New-Fixture 'missing-runtime' -IncludeOptionalData
    Remove-Item -LiteralPath (Join-Path $missingRuntimeSource 'Qt6Core.dll') -Force
    $missingRuntimeTarget = Join-Path $testRoot 'missing-runtime-target'
    New-Item -ItemType Directory -Path $missingRuntimeTarget | Out-Null
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File (Join-Path $missingRuntimeSource $handoffScriptName) `
        -TargetRoot $missingRuntimeTarget -NonInteractive
    Assert-True ($LASTEXITCODE -ne 0) 'missing required runtime should fail'
    Assert-True (@(Get-Packages $missingRuntimeTarget).Count -eq 0) 'failed validation should not create a final package'

    Write-Host 'Portable handoff tests passed.' -ForegroundColor Green
} finally {
    if (Test-Path -LiteralPath $testRoot) {
        Remove-Item -LiteralPath $testRoot -Recurse -Force
    }
}
