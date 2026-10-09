# Shared by the build and test scripts: the fixed Qt 6.5.3 / MinGW 11.2
# toolchain (never an unqualified system qmake, mingw32-make or g++).
# Dot-source this file, then call Initialize-QtToolchain once.

function Initialize-QtToolchain {
    param(
        [Parameter(Mandatory)][string]$QtRoot,
        [Parameter(Mandatory)][string]$MingwRoot,
        [switch]$Deploy
    )
    $tools = [ordered]@{
        qmake = Join-Path $QtRoot 'bin\qmake.exe'
        make  = Join-Path $MingwRoot 'bin\mingw32-make.exe'
    }
    if ($Deploy) { $tools.deploy = Join-Path $QtRoot 'bin\windeployqt.exe' }
    foreach ($tool in $tools.Values) {
        if (-not (Test-Path -LiteralPath $tool)) { throw "Required Qt tool was not found: $tool" }
    }
    $tools.qtRoot = $QtRoot
    $tools.mingwRoot = $MingwRoot
    $env:PATH = "$MingwRoot\bin;$QtRoot\bin;$env:PATH"
    return [pscustomobject]$tools
}

# Runs qmake for $Project into $BuildDir (created when missing).
function Invoke-QMake {
    param(
        [Parameter(Mandatory)]$Tools,
        [Parameter(Mandatory)][string]$Project,
        [Parameter(Mandatory)][string]$BuildDir,
        [string[]]$Arguments = @()
    )
    New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null
    Push-Location $BuildDir
    try {
        & $Tools.qmake -o Makefile $Project -spec win32-g++ @Arguments
        if ($LASTEXITCODE -ne 0) { throw "qmake failed with exit code $LASTEXITCODE" }
    } finally {
        Pop-Location
    }
}

# Builds a qmake-generated Makefile in $BuildDir, optionally from clean.
function Invoke-Make {
    param(
        [Parameter(Mandatory)]$Tools,
        [Parameter(Mandatory)][string]$BuildDir,
        [string]$Makefile = 'Makefile',
        [switch]$Clean
    )
    Push-Location $BuildDir
    try {
        if ($Clean) {
            & $Tools.make -f $Makefile clean
            if ($LASTEXITCODE -ne 0) { throw "clean failed with exit code $LASTEXITCODE" }
        }
        & $Tools.make -f $Makefile -j4
        if ($LASTEXITCODE -ne 0) { throw "build failed with exit code $LASTEXITCODE" }
    } finally {
        Pop-Location
    }
}

# The MinGW C++ runtime DLLs the executable needs next to it.
function Copy-MingwRuntime {
    param([Parameter(Mandatory)]$Tools, [Parameter(Mandatory)][string]$Destination)
    foreach ($runtime in @('libgcc_s_seh-1.dll', 'libstdc++-6.dll', 'libwinpthread-1.dll')) {
        Copy-Item -LiteralPath (Join-Path $Tools.mingwRoot "bin\$runtime") -Destination $Destination -Force
    }
}

# The portable hand-off helpers next to the executable; the PowerShell
# implementation is hidden so operators start the .cmd.
function Copy-PortableAssets {
    param([Parameter(Mandatory)][string]$ProjectRoot, [Parameter(Mandatory)][string]$Destination)
    $assets = Join-Path $ProjectRoot 'portable'
    if (-not (Test-Path -LiteralPath $assets -PathType Container)) {
        throw "Portable handoff assets were not found: $assets"
    }
    Get-ChildItem -LiteralPath $assets -File | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination $Destination -Force
    }
    $implementation = Get-ChildItem -LiteralPath $Destination -File -Filter '*.ps1' | Select-Object -First 1
    if ($null -ne $implementation) {
        $implementation.Attributes = $implementation.Attributes -bor [System.IO.FileAttributes]::Hidden
    }
}
