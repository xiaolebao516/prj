param(
    [string]$qtRoot = "D:\QT6.5.3\6.5.3\mingw_64",
    [string]$mingwRoot = "D:\QT6.5.3\Tools\mingw1120_64"
)
$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
. (Join-Path $projectRoot "scripts\toolchain.ps1")
$tools = Initialize-QtToolchain -QtRoot $qtRoot -MingwRoot $mingwRoot

$buildDir = Join-Path $projectRoot "build\release"
$exePath = Join-Path $buildDir "release\BoneDensity.exe"
$outputDir = Split-Path -Parent $exePath

Invoke-QMake -Tools $tools -Project (Join-Path $projectRoot "BoneDensity.pro") -BuildDir $buildDir `
             -Arguments @("CONFIG-=debug", "CONFIG+=release")

$releaseMakefile = Get-Content -LiteralPath (Join-Path $buildDir "Makefile.Release") -Raw
if ($releaseMakefile -notmatch '(?m)(^|\s)-O2(\s|$)') {
    throw "Release Makefile does not use -O2."
}
if ($releaseMakefile -match '(^|\s)(-O0|-O3|-Ofast|-ffast-math|-funsafe-math-optimizations|-ffinite-math-only|-fassociative-math|-flto|-march=native)(\s|$)') {
    throw "Release Makefile contains an unsupported optimization flag: $($Matches[2])"
}

Invoke-Make -Tools $tools -BuildDir $buildDir -Makefile "Makefile.Release" -Clean

# This Qt 6.5.3 installation reports its MinGW plugins as "debug" to
# windeployqt even though they are the only compatible runtime plugins.
# Copy the audited runtime whitelist explicitly so Release packaging is
# deterministic on this development machine.
$qtRuntimeFiles = @(
    "Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll", "Qt6Xml.dll",
    "Qt6SerialPort.dll", "Qt6PrintSupport.dll", "Qt6Charts.dll",
    "Qt6Network.dll", "Qt6OpenGL.dll", "Qt6OpenGLWidgets.dll",
    "Qt6Svg.dll", "D3Dcompiler_47.dll", "opengl32sw.dll"
)
foreach ($runtime in $qtRuntimeFiles) {
    $source = Join-Path $qtRoot "bin\$runtime"
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Required Qt runtime was not found: $source"
    }
    Copy-Item -LiteralPath $source -Destination $outputDir -Force
}
$qtPluginFiles = @(
    "generic\qtuiotouchplugin.dll",
    "iconengines\qsvgicon.dll",
    "imageformats\qgif.dll", "imageformats\qicns.dll",
    "imageformats\qico.dll", "imageformats\qjpeg.dll",
    "imageformats\qsvg.dll", "imageformats\qtga.dll",
    "imageformats\qtiff.dll", "imageformats\qwbmp.dll",
    "imageformats\qwebp.dll",
    "networkinformation\qnetworklistmanager.dll",
    "platforms\qwindows.dll",
    "styles\qwindowsvistastyle.dll",
    "tls\qcertonlybackend.dll", "tls\qopensslbackend.dll",
    "tls\qschannelbackend.dll"
)
foreach ($relativePath in $qtPluginFiles) {
    $source = Join-Path $qtRoot "plugins\$relativePath"
    $destination = Join-Path $outputDir $relativePath
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Required Qt plugin was not found: $source"
    }
    New-Item -ItemType Directory -Path (Split-Path -Parent $destination) -Force | Out-Null
    Copy-Item -LiteralPath $source -Destination $destination -Force
}

Copy-MingwRuntime -Tools $tools -Destination $outputDir
Copy-PortableAssets -ProjectRoot $projectRoot -Destination $outputDir

$exeHash = (Get-FileHash -LiteralPath $exePath -Algorithm SHA256).Hash
$buildInfo = @(
    "BuildType=Release",
    "Optimization=O2",
    "Qt=6.5.3 MinGW 64-bit",
    "MinGW=11.2",
    "ExecutableSHA256=$exeHash"
)
[System.IO.File]::WriteAllLines(
    (Join-Path $outputDir "release-build-info.txt"),
    $buildInfo,
    [System.Text.UTF8Encoding]::new($false)
)

Write-Host "Release build completed: $exePath"
