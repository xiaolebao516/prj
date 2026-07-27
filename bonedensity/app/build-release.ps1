$ErrorActionPreference = "Stop"

$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$qtRoot = "D:\Qt\6.5.3\mingw_64"
$mingwRoot = "D:\Qt\Tools\mingw1120_64"
$buildDir = Join-Path $projectRoot "build\release"
$exePath = Join-Path $buildDir "release\BoneDensity.exe"
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
    & $qmake -o Makefile "..\..\BoneDensity.pro" -spec win32-g++ "CONFIG-=debug" "CONFIG+=release"
    if ($LASTEXITCODE -ne 0) { throw "qmake failed with exit code $LASTEXITCODE" }

    $releaseMakefile = Get-Content -LiteralPath "Makefile.Release" -Raw
    if ($releaseMakefile -notmatch '(?m)(^|\s)-O2(\s|$)') {
        throw "Release Makefile does not use -O2."
    }
    if ($releaseMakefile -match '(^|\s)(-O0|-O3|-Ofast|-ffast-math|-funsafe-math-optimizations|-ffinite-math-only|-fassociative-math|-flto|-march=native)(\s|$)') {
        throw "Release Makefile contains an unsupported optimization flag: $($Matches[2])"
    }

    & $make -f Makefile.Release clean
    if ($LASTEXITCODE -ne 0) { throw "clean failed with exit code $LASTEXITCODE" }

    & $make -f Makefile.Release -j4
    if ($LASTEXITCODE -ne 0) { throw "build failed with exit code $LASTEXITCODE" }

    # This Qt 6.5.3 installation reports its MinGW plugins as "debug" to
    # windeployqt even though they are the only compatible runtime plugins.
    # Copy the audited runtime whitelist explicitly so Release packaging is
    # deterministic on this development machine.
    $outputDir = Split-Path -Parent $exePath
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

    $exeHash = (Get-FileHash -LiteralPath $exePath -Algorithm SHA256).Hash
    $buildInfo = @(
        "BuildType=Release",
        "Optimization=O2",
        "Qt=6.5.3 MinGW 64-bit",
        "MinGW=11.2",
        "ExecutableSHA256=$exeHash"
    )
    [System.IO.File]::WriteAllLines(
        (Join-Path (Split-Path -Parent $exePath) "release-build-info.txt"),
        $buildInfo,
        [System.Text.UTF8Encoding]::new($false)
    )
} finally {
    Pop-Location
}

Write-Host "Release build completed: $exePath"
