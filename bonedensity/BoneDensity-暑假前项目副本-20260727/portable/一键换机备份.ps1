param(
    [string]$TargetRoot,
    [switch]$NonInteractive
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$handoffScriptPath = $PSCommandPath
$runtimeRoot = $PSScriptRoot
if (-not (Test-Path -LiteralPath (Join-Path $runtimeRoot 'BoneDensity.exe') -PathType Leaf)) {
    $parentRoot = Split-Path -Parent $runtimeRoot
    if (Test-Path -LiteralPath (Join-Path $parentRoot 'BoneDensity.exe') -PathType Leaf) {
        $runtimeRoot = $parentRoot
    }
}
$supportDirectoryName = '运行组件'
$requiredRuntimeFiles = @(
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
$requiredPluginFiles = @(
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
$requiredDataFiles = @('accounts.xml', 'calibration.xml')
$optionalDataFiles = @('patients.xml', 'measurements.xml', 'angle_features.csv')
$handoffFiles = @('一键换机备份.cmd', '换机说明.txt', '使用说明.txt')

function Test-YesNo([string]$Prompt) {
    while ($true) {
        $answer = (Read-Host "$Prompt (Y/N)").Trim()
        if ($answer -match '^[Yy]$') { return $true }
        if ($answer -match '^[Nn]$') { return $false }
        Write-Host '请输入 Y 或 N。' -ForegroundColor Yellow
    }
}

function Get-RemovableDrives {
    try {
        return @(Get-CimInstance -ClassName Win32_LogicalDisk -Filter 'DriveType = 2' |
            Sort-Object DeviceID |
            Select-Object DeviceID, VolumeName, Size, FreeSpace)
    } catch {
        return @(Get-WmiObject -Class Win32_LogicalDisk -Filter 'DriveType = 2' |
            Sort-Object DeviceID |
            Select-Object DeviceID, VolumeName, Size, FreeSpace)
    }
}

function Get-UniquePackagePath([string]$Root) {
    $baseName = 'BoneDensity-换机包-' + (Get-Date -Format 'yyyyMMdd-HHmmss')
    $candidate = Join-Path $Root $baseName
    $suffix = 2
    while (Test-Path -LiteralPath $candidate) {
        $candidate = Join-Path $Root ("$baseName-$suffix")
        ++$suffix
    }
    return $candidate
}

function Assert-SourcePayload {
    if (-not [Environment]::Is64BitOperatingSystem) {
        throw '此换机包仅支持 64 位 Windows。'
    }
    foreach ($relativePath in $requiredRuntimeFiles + $requiredPluginFiles + $requiredDataFiles + $handoffFiles) {
        if (-not (Test-Path -LiteralPath (Join-Path $runtimeRoot $relativePath) -PathType Leaf)) {
            throw "运行目录缺少必需文件：$relativePath"
        }
    }
    if (-not (Test-Path -LiteralPath $handoffScriptPath -PathType Leaf)) {
        throw '找不到一键换机备份的内部脚本。'
    }
    if (Test-Path -LiteralPath (Join-Path $runtimeRoot 'BoneDensity.instance.lock')) {
        throw '检测到骨密度程序仍在运行或上次未正常退出。请先关闭程序。若确认任务管理器中没有 BoneDensity.exe 仍出现此提示，可删除软件目录中的 BoneDensity.instance.lock 后重试。'
    }
    if (Test-Path -LiteralPath (Join-Path $runtimeRoot 'patients.xml.txn') -PathType Leaf) {
        throw '检测到未完成的档案保存。请先启动骨密度软件完成自动恢复，正常退出后再重试换机。'
    }
    if ((Test-Path -LiteralPath (Join-Path $runtimeRoot 'measurements.xml') -PathType Leaf) -and
        -not (Test-Path -LiteralPath (Join-Path $runtimeRoot 'patients.xml') -PathType Leaf)) {
        throw '检测记录存在，但患者档案缺失。为避免带走不完整数据，换机已中止。'
    }
}

function Copy-PayloadFile([string]$RelativePath, [string]$PackagePath) {
    $sourcePath = Join-Path $runtimeRoot $RelativePath
    $destinationPath = Join-Path $PackagePath $RelativePath
    New-Item -ItemType Directory -Path (Split-Path -Parent $destinationPath) -Force | Out-Null
    Copy-Item -LiteralPath $sourcePath -Destination $destinationPath -Force
}

function Write-Manifest([string]$PackagePath) {
    $manifestPath = Join-Path $PackagePath '换机包清单.txt'
    $lines = @(
        'BoneDensity portable handoff package',
        'Package format: 1',
        ('Created: ' + (Get-Date -Format o)),
        'Supported OS: Windows 10 1809 or newer / Windows 11 x64',
        'Data storage format: unchanged XML',
        ''
    )
    Get-ChildItem -LiteralPath $PackagePath -Recurse -Force -File |
        Where-Object { $_.FullName -ne $manifestPath } |
        Sort-Object FullName |
        ForEach-Object {
        $relativePath = $_.FullName.Substring($PackagePath.Length + 1)
        $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash
        $lines += "$relativePath SHA256 $hash"
    }
    [System.IO.File]::WriteAllLines($manifestPath, $lines, [System.Text.UTF8Encoding]::new($true))
}

function Assert-Manifest([string]$PackagePath) {
    $manifestPath = Join-Path $PackagePath '换机包清单.txt'
    $entries = @{}
    foreach ($line in Get-Content -LiteralPath $manifestPath) {
        if ($line -notmatch '^(?<Path>.+) SHA256 (?<Hash>[0-9A-Fa-f]{64})$') { continue }
        if ($entries.ContainsKey($Matches.Path)) {
            throw "换机包清单重复记录：$($Matches.Path)"
        }
        $entries[$Matches.Path] = $Matches.Hash.ToUpperInvariant()
    }

    $payloadFiles = @(Get-ChildItem -LiteralPath $PackagePath -Recurse -Force -File |
        Where-Object { $_.FullName -ne $manifestPath })
    if ($entries.Count -ne $payloadFiles.Count) {
        throw "换机包清单数量不一致：清单 $($entries.Count) 项，实际 $($payloadFiles.Count) 项。"
    }
    foreach ($file in $payloadFiles) {
        $relativePath = $file.FullName.Substring($PackagePath.Length + 1)
        if (-not $entries.ContainsKey($relativePath)) {
            throw "换机包清单漏记文件：$relativePath"
        }
        $actualHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $file.FullName).Hash
        if ($entries[$relativePath] -ne $actualHash) {
            throw "换机包清单校验失败：$relativePath"
        }
    }
}

function Start-PortableHandoff {
    Assert-SourcePayload

    if (-not $NonInteractive) {
        Write-Host ''
        Write-Host '骨密度仪换机备份' -ForegroundColor Cyan
        Write-Host '请先关闭骨密度程序，再插入 U 盘。'
        if (-not (Test-YesNo '已关闭程序并插入 U 盘吗？')) {
            Write-Host '已取消；未复制任何文件。'
            return
        }
    }

    if ([string]::IsNullOrWhiteSpace($TargetRoot)) {
        $drives = Get-RemovableDrives
        if ($drives.Count -eq 0) {
            throw '没有检测到可移动磁盘。请插入 U 盘后重试。'
        }
        Write-Host ''
        Write-Host '检测到以下可移动磁盘：'
        foreach ($drive in $drives) {
            $name = if ([string]::IsNullOrWhiteSpace($drive.VolumeName)) { '未命名' } else { $drive.VolumeName }
            Write-Host ("  {0}  {1}" -f $drive.DeviceID, $name)
        }
        $selection = (Read-Host '请输入要使用的盘符，例如 E:').Trim().TrimEnd('\\')
        if ($selection -notmatch '^[A-Za-z]:$') {
            throw '盘符格式无效。'
        }
        $selection = $selection.ToUpperInvariant()
        if (-not (@($drives.DeviceID) -contains $selection)) {
            throw '请选择上面列出的可移动磁盘盘符。'
        }
        $TargetRoot = $selection + '\'
        if (-not (Test-Path -LiteralPath $TargetRoot -PathType Container)) {
            throw '所选盘符不可用。'
        }
        if (-not $NonInteractive -and -not (Test-YesNo "确认复制到 $TargetRoot 吗？")) {
            Write-Host '已取消；未复制任何文件。'
            return
        }
    }

    if (-not (Test-Path -LiteralPath $TargetRoot -PathType Container)) {
        throw "目标目录不可用：$TargetRoot"
    }

    $handoffLockPath = Join-Path $runtimeRoot 'BoneDensity.instance.lock'
    $handoffLockAcquired = $false
    try {
        try {
            New-Item -ItemType Directory -Path $handoffLockPath -ErrorAction Stop | Out-Null
            $handoffLockAcquired = $true
        } catch {
            throw '无法锁定运行目录，软件或另一个换机备份可能刚刚启动。请关闭后重试。'
        }

        $finalPath = Get-UniquePackagePath $TargetRoot
        $partialPath = $finalPath + '.partial'
        New-Item -ItemType Directory -Path $partialPath -ErrorAction Stop | Out-Null

        try {
            foreach ($relativePath in $requiredRuntimeFiles + $requiredPluginFiles +
                                          $requiredDataFiles + $handoffFiles) {
                Copy-PayloadFile $relativePath $partialPath
            }
            foreach ($relativePath in $optionalDataFiles) {
                if (Test-Path -LiteralPath (Join-Path $runtimeRoot $relativePath) -PathType Leaf) {
                    Copy-PayloadFile $relativePath $partialPath
                }
            }
            $supportDirectory = Join-Path $partialPath $supportDirectoryName
            New-Item -ItemType Directory -Path $supportDirectory -Force | Out-Null
            Copy-Item -LiteralPath $handoffScriptPath `
                -Destination (Join-Path $supportDirectory '一键换机备份.ps1') -Force

            foreach ($dataFile in $requiredDataFiles + $optionalDataFiles) {
                $sourcePath = Join-Path $runtimeRoot $dataFile
                $packagePath = Join-Path $partialPath $dataFile
                if (Test-Path -LiteralPath $sourcePath -PathType Leaf) {
                    if (-not (Test-Path -LiteralPath $packagePath -PathType Leaf)) {
                        throw "复制后的换机包缺少数据文件：$dataFile"
                    }
                    $sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $sourcePath).Hash
                    $packageHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $packagePath).Hash
                    if ($sourceHash -ne $packageHash) {
                        throw "数据文件校验失败：$dataFile"
                    }
                }
            }
            Write-Manifest $partialPath
            Assert-Manifest $partialPath
            Move-Item -LiteralPath $partialPath -Destination $finalPath -ErrorAction Stop
            Write-Host ''
            Write-Host '换机包已创建：' -ForegroundColor Green
            Write-Host $finalPath -ForegroundColor Green
            Write-Host '请将整个文件夹复制到新电脑的可写位置，然后双击 BoneDensity.exe。'
        } catch {
            Write-Host "换机包未完成：$($_.Exception.Message)" -ForegroundColor Red
            Write-Host "未完成目录保留在：$partialPath" -ForegroundColor Yellow
            throw
        }
    } finally {
        if ($handoffLockAcquired -and
            (Test-Path -LiteralPath $handoffLockPath -PathType Container)) {
            Remove-Item -LiteralPath $handoffLockPath -Force
        }
    }
}

if ($MyInvocation.InvocationName -ne '.') {
    try {
        Start-PortableHandoff
        exit 0
    } catch {
        Write-Error $_.Exception.Message
        exit 1
    }
}
