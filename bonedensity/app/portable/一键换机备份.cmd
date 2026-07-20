@echo off
setlocal
chcp 65001 >nul
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "$handoff = Get-ChildItem -LiteralPath '%~dp0' -Filter '*.ps1' | Select-Object -First 1; if ($null -eq $handoff) { exit 1 }; & $handoff.FullName; exit $LASTEXITCODE"
set "EXIT_CODE=%ERRORLEVEL%"
if not "%EXIT_CODE%"=="0" (
    echo.
    echo 换机包未创建。请根据上面的提示处理后重试。
)
pause
exit /b %EXIT_CODE%
