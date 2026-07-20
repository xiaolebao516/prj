param(
    [string]$TargetRoot,
    [switch]$NonInteractive
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$runtimeRoot = $PSScriptRoot
$requiredRuntimeFiles = @(
    'BoneDensity.exe',
    'Qt6Core.dll',
    'Qt6Gui.dll',
    'Qt6Widgets.dll',
    'Qt6Xml.dll',
    'Qt6SerialPort.dll',
    'Qt6PrintSupport.dll',
    'libgcc_s_seh-1.dll',
    'libstdc++-6.dll',
    'libwinpthread-1.dll'
)
$requiredPluginFiles = @('platforms\qwindows.dll')
$requiredDataFiles = @('accounts.xml', 'calibration.xml')
$optionalDataFiles = @('patients.xml', 'measurements.xml', 'angle_features.csv')
$pluginDirectories = @('platforms', 'styles', 'imageformats', 'iconengines', 'networkinformation', 'tls', 'generic')
$handoffFiles = @('一键换机备份.cmd', '一键换机备份.ps1', '换机说明.txt')

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

function Invoke-Robocopy([string]$Source, [string]$Destination, [string[]]$Files) {
    & robocopy.exe $Source $Destination @Files /R:1 /W:1 /COPY:DAT /DCOPY:T /NFL /NDL /NJH /NJS /NP | Out-Host
    if ($LASTEXITCODE -gt 7) {
        throw "复制失败：robocopy 返回代码 $LASTEXITCODE。"
    }
}

function Invoke-DirectoryCopy([string]$Source, [string]$Destination) {
    & robocopy.exe $Source $Destination /E /R:1 /W:1 /COPY:DAT /DCOPY:T /NFL /NDL /NJH /NJS /NP | Out-Host
    if ($LASTEXITCODE -gt 7) {
        throw "复制插件目录失败：robocopy 返回代码 $LASTEXITCODE。"
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
    foreach ($directory in $pluginDirectories) {
        if ((Test-Path -LiteralPath (Join-Path $runtimeRoot $directory)) -and
            -not (Test-Path -LiteralPath (Join-Path $runtimeRoot $directory) -PathType Container)) {
            throw "插件路径不是目录：$directory"
        }
    }
}

function Write-Manifest([string]$PackagePath, [string[]]$CopiedDataFiles) {
    $manifestPath = Join-Path $PackagePath '换机包清单.txt'
    $lines = @(
        'BoneDensity portable handoff package',
        'Package format: 1',
        ('Created: ' + (Get-Date -Format o)),
        'Supported OS: Windows 10/11 x64',
        'Data storage format: unchanged XML',
        ''
    )
    foreach ($relativePath in @('BoneDensity.exe') + $CopiedDataFiles) {
        $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $PackagePath $relativePath)).Hash
        $lines += "$relativePath SHA256 $hash"
    }
    [System.IO.File]::WriteAllLines($manifestPath, $lines, [System.Text.UTF8Encoding]::new($false))
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
        $TargetRoot = $selection + '\\'
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

    $finalPath = Get-UniquePackagePath $TargetRoot
    $partialPath = $finalPath + '.partial'
    New-Item -ItemType Directory -Path $partialPath -ErrorAction Stop | Out-Null

    try {
        $topLevelFiles = @('BoneDensity.exe', '*.dll') + $requiredDataFiles + $optionalDataFiles + $handoffFiles
        Invoke-Robocopy $runtimeRoot $partialPath $topLevelFiles
        foreach ($directory in $pluginDirectories) {
            $sourceDirectory = Join-Path $runtimeRoot $directory
            if (Test-Path -LiteralPath $sourceDirectory -PathType Container) {
                Invoke-DirectoryCopy $sourceDirectory (Join-Path $partialPath $directory)
            }
        }

        $copiedDataFiles = @()
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
                $copiedDataFiles += $dataFile
            }
        }
        Write-Manifest $partialPath $copiedDataFiles
        $internalScript = Join-Path $partialPath '一键换机备份.ps1'
        if (Test-Path -LiteralPath $internalScript -PathType Leaf) {
            $item = Get-Item -LiteralPath $internalScript
            $item.Attributes = $item.Attributes -bor [System.IO.FileAttributes]::Hidden
        }
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
