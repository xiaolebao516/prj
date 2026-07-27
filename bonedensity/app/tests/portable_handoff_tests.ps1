$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$projectRoot = Split-Path -Parent $PSScriptRoot
$assetRoot = Join-Path $projectRoot 'portable'
$handoffScriptName = '一键换机备份.ps1'
$supportDirectoryName = '运行组件'
$testRoot = Join-Path ([System.IO.Path]::GetTempPath()) ('骨密度 换机测试-' + [guid]::NewGuid().ToString('N'))
$launcherText = Get-Content -LiteralPath (Join-Path $assetRoot '一键换机备份.cmd') -Raw
$launcherBytes = [System.IO.File]::ReadAllBytes((Join-Path $assetRoot '一键换机备份.cmd'))
$runtimeFixtureFiles = @(
    'BoneDensity.exe',
    'Qt6Core.dll',
    'Qt6Gui.dll',
    'Qt6Widgets.dll',
    'Qt6Xml.dll',
    'Qt6SerialPort.dll',
    'Qt6PrintSupport.dll',
    'Qt6Charts.dll',
    'Qt6Network.dll',
    'Qt6OpenGL.dll',
    'Qt6OpenGLWidgets.dll',
    'Qt6Svg.dll',
    'D3Dcompiler_47.dll',
    'opengl32sw.dll',
    'libgcc_s_seh-1.dll',
    'libstdc++-6.dll',
    'libwinpthread-1.dll'
)
$pluginFixtureFiles = @(
    'generic\qtuiotouchplugin.dll',
    'iconengines\qsvgicon.dll',
    'imageformats\qgif.dll',
    'imageformats\qicns.dll',
    'imageformats\qico.dll',
    'imageformats\qjpeg.dll',
    'imageformats\qsvg.dll',
    'imageformats\qtga.dll',
    'imageformats\qtiff.dll',
    'imageformats\qwbmp.dll',
    'imageformats\qwebp.dll',
    'networkinformation\qnetworklistmanager.dll',
    'platforms\qwindows.dll',
    'styles\qwindowsvistastyle.dll',
    'tls\qcertonlybackend.dll',
    'tls\qopensslbackend.dll',
    'tls\qschannelbackend.dll'
)

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw "Assertion failed: $Message" }
}

function New-Fixture([string]$Name, [switch]$IncludeOptionalData) {
    $root = Join-Path $testRoot $Name
    New-Item -ItemType Directory -Path $root -Force | Out-Null
    foreach ($asset in @('一键换机备份.cmd', '一键换机备份.ps1', '换机说明.txt', '使用说明.txt')) {
        Copy-Item -LiteralPath (Join-Path $assetRoot $asset) -Destination $root
    }
    foreach ($file in $runtimeFixtureFiles + @('accounts.xml', 'calibration.xml')) {
        Set-Content -LiteralPath (Join-Path $root $file) -Value "fixture:$file" -Encoding utf8
    }
    foreach ($relativePath in $pluginFixtureFiles) {
        $path = Join-Path $root $relativePath
        New-Item -ItemType Directory -Path (Split-Path -Parent $path) -Force | Out-Null
        Set-Content -LiteralPath $path -Value "fixture:$relativePath" -Encoding utf8
    }
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

function Invoke-CmdCancel([string]$Source) {
    $startInfo = [System.Diagnostics.ProcessStartInfo]::new()
    $startInfo.FileName = $env:ComSpec
    $startInfo.Arguments = '/d /c call "一键换机备份.cmd"'
    $startInfo.WorkingDirectory = $Source
    $startInfo.UseShellExecute = $false
    $startInfo.CreateNoWindow = $true
    $startInfo.RedirectStandardInput = $true
    $startInfo.RedirectStandardOutput = $true
    $startInfo.RedirectStandardError = $true
    $startInfo.StandardOutputEncoding = [System.Text.Encoding]::UTF8
    $startInfo.StandardErrorEncoding = [System.Text.Encoding]::UTF8
    $process = [System.Diagnostics.Process]::new()
    $process.StartInfo = $startInfo
    Assert-True ($process.Start()) 'CMD launcher should start'
    $process.StandardInput.WriteLine('N')
    $process.StandardInput.WriteLine('')
    $process.StandardInput.Close()
    if (-not $process.WaitForExit(15000)) {
        $process.Kill()
        throw 'CMD launcher did not finish after the N cancellation response'
    }
    $output = $process.StandardOutput.ReadToEnd() + $process.StandardError.ReadToEnd()
    Assert-True ($process.ExitCode -eq 0) `
        "CMD N cancellation should exit successfully; exit=$($process.ExitCode); output=$output"
    Assert-True ($output -match '已取消') 'CMD should reach the PowerShell N cancellation branch'
}

function Start-HandoffProcess([string]$Source, [string]$Target) {
    $startInfo = [System.Diagnostics.ProcessStartInfo]::new()
    $startInfo.FileName = 'powershell.exe'
    $startInfo.Arguments = (
        '-NoProfile -ExecutionPolicy Bypass -File "{0}" -TargetRoot "{1}" -NonInteractive' -f
        (Join-Path $Source $handoffScriptName), $Target
    )
    $startInfo.UseShellExecute = $false
    $startInfo.CreateNoWindow = $true
    $startInfo.RedirectStandardOutput = $true
    $startInfo.RedirectStandardError = $true
    $process = [System.Diagnostics.Process]::new()
    $process.StartInfo = $startInfo
    Assert-True ($process.Start()) 'handoff child process should start'
    return $process
}

try {
    Assert-True ($launcherText -notmatch '-Command' -and $launcherText -match 'powershell\.exe') 'launcher should invoke PowerShell without an inline command string'
    Assert-True ([System.Text.Encoding]::ASCII.GetString($launcherBytes) -match "`r`n") 'launcher should use Windows CRLF line endings'
    New-Item -ItemType Directory -Path $testRoot -Force | Out-Null

    $successSource = New-Fixture 'success' -IncludeOptionalData
    Set-Content -LiteralPath (Join-Path $successSource 'stale-debug.dll') `
        -Value 'must not be copied' -Encoding utf8
    Set-Content -LiteralPath (Join-Path $successSource 'platforms\unused.dll') `
        -Value 'must not be copied' -Encoding utf8
    Invoke-CmdCancel $successSource
    $successTarget = Join-Path $testRoot 'target'
    New-Item -ItemType Directory -Path $successTarget | Out-Null
    Invoke-Handoff $successSource $successTarget
    $firstPackage = @(Get-Packages $successTarget)
    Assert-True ($firstPackage.Count -eq 1) 'first package should be created'
    Assert-True (-not (Test-Path -LiteralPath (Join-Path $successSource 'BoneDensity.instance.lock'))) `
        'successful handoff should release its temporary lock'
    $packagePath = $firstPackage[0].FullName
    foreach ($file in @('BoneDensity.exe', 'accounts.xml', 'calibration.xml', 'patients.xml', 'measurements.xml', '换机说明.txt', '使用说明.txt', '换机包清单.txt')) {
        Assert-True (Test-Path -LiteralPath (Join-Path $packagePath $file) -PathType Leaf) "package should contain $file"
    }
    $internalScript = Join-Path $packagePath "$supportDirectoryName\$handoffScriptName"
    Assert-True (Test-Path -LiteralPath $internalScript -PathType Leaf) `
        'package should keep its PowerShell implementation under the support directory'
    Assert-True (-not (Test-Path -LiteralPath (Join-Path $packagePath $handoffScriptName))) `
        'package should not show a second top-level handoff script'
    $sourceHash = (Get-FileHash -LiteralPath (Join-Path $successSource 'accounts.xml') -Algorithm SHA256).Hash
    $packageHash = (Get-FileHash -LiteralPath (Join-Path $packagePath 'accounts.xml') -Algorithm SHA256).Hash
    Assert-True ($sourceHash -eq $packageHash) 'accounts.xml hash should be preserved'
    Assert-True ((Get-Content -LiteralPath (Join-Path $packagePath '换机包清单.txt') -Raw) -match 'accounts.xml SHA256') 'manifest should list account hash'
    $manifestBytes = [System.IO.File]::ReadAllBytes((Join-Path $packagePath '换机包清单.txt'))
    Assert-True ($manifestBytes.Length -ge 3 -and
        $manifestBytes[0] -eq 0xEF -and $manifestBytes[1] -eq 0xBB -and
        $manifestBytes[2] -eq 0xBF) 'handoff manifest should use a UTF-8 BOM'
    Assert-True (-not (Test-Path -LiteralPath (Join-Path $packagePath 'stale-debug.dll'))) `
        'handoff should not copy unlisted DLLs'
    Assert-True (-not (Test-Path -LiteralPath (Join-Path $packagePath 'platforms\unused.dll'))) `
        'handoff should not copy unlisted plugin files'

    $manifestPath = Join-Path $packagePath '换机包清单.txt'
    $manifestEntries = @{}
    foreach ($line in Get-Content -LiteralPath $manifestPath) {
        if ($line -notmatch '^(?<Path>.+) SHA256 (?<Hash>[0-9A-Fa-f]{64})$') { continue }
        Assert-True (-not $manifestEntries.ContainsKey($Matches.Path)) `
            "handoff manifest should not repeat $($Matches.Path)"
        $manifestEntries[$Matches.Path] = $Matches.Hash.ToUpperInvariant()
    }
    $payloadFiles = @(Get-ChildItem -LiteralPath $packagePath -Recurse -Force -File |
        Where-Object { $_.FullName -ne $manifestPath })
    Assert-True ($manifestEntries.Count -eq $payloadFiles.Count) `
        'handoff manifest entry count should match every payload file'
    foreach ($payload in $payloadFiles) {
        $relative = $payload.FullName.Substring($packagePath.Length + 1)
        Assert-True ($manifestEntries.ContainsKey($relative)) `
            "handoff manifest should list $relative"
        $actualHash = (Get-FileHash -LiteralPath $payload.FullName -Algorithm SHA256).Hash
        Assert-True ($manifestEntries[$relative] -eq $actualHash) `
            "handoff manifest hash should match $relative"
    }

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
    Remove-Item -LiteralPath (Join-Path $missingRuntimeSource 'Qt6Charts.dll') -Force
    $missingRuntimeTarget = Join-Path $testRoot 'missing-runtime-target'
    New-Item -ItemType Directory -Path $missingRuntimeTarget | Out-Null
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File (Join-Path $missingRuntimeSource $handoffScriptName) `
        -TargetRoot $missingRuntimeTarget -NonInteractive
    Assert-True ($LASTEXITCODE -ne 0) 'missing required runtime should fail'
    Assert-True (@(Get-Packages $missingRuntimeTarget).Count -eq 0) 'failed validation should not create a final package'

    $transactionSource = New-Fixture 'pending-transaction' -IncludeOptionalData
    Set-Content -LiteralPath (Join-Path $transactionSource 'patients.xml.txn') `
        -Value '<patientDataTransaction version="1" />' -Encoding utf8
    $transactionTarget = Join-Path $testRoot 'pending-transaction-target'
    New-Item -ItemType Directory -Path $transactionTarget | Out-Null
    & powershell.exe -NoProfile -ExecutionPolicy Bypass -File (Join-Path $transactionSource $handoffScriptName) `
        -TargetRoot $transactionTarget -NonInteractive
    Assert-True ($LASTEXITCODE -ne 0) 'pending patient transaction should block handoff'
    Assert-True (@(Get-Packages $transactionTarget).Count -eq 0) `
        'blocked transaction handoff should not create a final package'

    $lockedSource = New-Fixture 'running-lock' -IncludeOptionalData
    Set-Content -LiteralPath (Join-Path $lockedSource 'BoneDensity.instance.lock') `
        -Value 'fixture lock' -Encoding utf8
    $lockedTarget = Join-Path $testRoot 'running-lock-target'
    New-Item -ItemType Directory -Path $lockedTarget | Out-Null
    & powershell.exe -NoProfile -ExecutionPolicy Bypass `
        -File (Join-Path $lockedSource $handoffScriptName) `
        -TargetRoot $lockedTarget -NonInteractive
    Assert-True ($LASTEXITCODE -ne 0) 'application lock should block handoff'
    Assert-True (@(Get-Packages $lockedTarget).Count -eq 0) `
        'locked handoff should not create a final package'

    $directoryLockedSource = New-Fixture 'running-directory-lock' -IncludeOptionalData
    New-Item -ItemType Directory `
        -Path (Join-Path $directoryLockedSource 'BoneDensity.instance.lock') | Out-Null
    $directoryLockedTarget = Join-Path $testRoot 'running-directory-lock-target'
    New-Item -ItemType Directory -Path $directoryLockedTarget | Out-Null
    & powershell.exe -NoProfile -ExecutionPolicy Bypass `
        -File (Join-Path $directoryLockedSource $handoffScriptName) `
        -TargetRoot $directoryLockedTarget -NonInteractive
    Assert-True ($LASTEXITCODE -ne 0) 'handoff lock directory should block a second handoff'
    Assert-True (@(Get-Packages $directoryLockedTarget).Count -eq 0) `
        'second handoff should not create a final package'

    $concurrentSource = New-Fixture 'concurrent-handoff' -IncludeOptionalData
    $largeRuntime = [System.IO.File]::OpenWrite(
        (Join-Path $concurrentSource 'opengl32sw.dll')
    )
    try {
        $largeRuntime.SetLength(32MB)
    } finally {
        $largeRuntime.Dispose()
    }
    $concurrentTargetA = Join-Path $testRoot 'concurrent-target-a'
    $concurrentTargetB = Join-Path $testRoot 'concurrent-target-b'
    New-Item -ItemType Directory -Path $concurrentTargetA | Out-Null
    New-Item -ItemType Directory -Path $concurrentTargetB | Out-Null
    $concurrentA = Start-HandoffProcess $concurrentSource $concurrentTargetA
    $concurrentB = Start-HandoffProcess $concurrentSource $concurrentTargetB
    Assert-True ($concurrentA.WaitForExit(30000)) 'first concurrent handoff should finish'
    Assert-True ($concurrentB.WaitForExit(30000)) 'second concurrent handoff should finish'
    $concurrentOutputA = $concurrentA.StandardOutput.ReadToEnd() +
        $concurrentA.StandardError.ReadToEnd()
    $concurrentOutputB = $concurrentB.StandardOutput.ReadToEnd() +
        $concurrentB.StandardError.ReadToEnd()
    $concurrentExitCodes = @($concurrentA.ExitCode, $concurrentB.ExitCode) | Sort-Object
    Assert-True ($concurrentExitCodes[0] -eq 0 -and $concurrentExitCodes[1] -ne 0) `
        "exactly one concurrent handoff should succeed; A=$($concurrentA.ExitCode) $concurrentOutputA; B=$($concurrentB.ExitCode) $concurrentOutputB"
    $concurrentPackageCountA = @(Get-Packages $concurrentTargetA).Count
    $concurrentPackageCountB = @(Get-Packages $concurrentTargetB).Count
    Assert-True (($concurrentPackageCountA + $concurrentPackageCountB) -eq 1) `
        "concurrent handoffs should create exactly one final package; countA=$concurrentPackageCountA, countB=$concurrentPackageCountB; A=$concurrentOutputA; B=$concurrentOutputB"
    Assert-True (-not (Test-Path -LiteralPath (
            Join-Path $concurrentSource 'BoneDensity.instance.lock'
        ))) 'concurrent handoff owner should release its lock'

    $orphanMeasurementsSource = New-Fixture 'orphan-measurements'
    Set-Content -LiteralPath (Join-Path $orphanMeasurementsSource 'measurements.xml') `
        -Value '<measurements version="1" />' -Encoding utf8
    $orphanMeasurementsTarget = Join-Path $testRoot 'orphan-measurements-target'
    New-Item -ItemType Directory -Path $orphanMeasurementsTarget | Out-Null
    & powershell.exe -NoProfile -ExecutionPolicy Bypass `
        -File (Join-Path $orphanMeasurementsSource $handoffScriptName) `
        -TargetRoot $orphanMeasurementsTarget -NonInteractive
    Assert-True ($LASTEXITCODE -ne 0) 'measurements without patients should block handoff'
    Assert-True (@(Get-Packages $orphanMeasurementsTarget).Count -eq 0) `
        'inconsistent handoff source should not create a final package'

    Write-Host 'Portable handoff tests passed.' -ForegroundColor Green
} finally {
    if (Test-Path -LiteralPath $testRoot) {
        Remove-Item -LiteralPath $testRoot -Recurse -Force
    }
}
