@echo off
setlocal
chcp 65001 >nul
set "HANDOFF_SCRIPT=%~dp0运行组件\一键换机备份.ps1"
if not exist "%HANDOFF_SCRIPT%" set "HANDOFF_SCRIPT=%~dp0一键换机备份.ps1"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%HANDOFF_SCRIPT%"
set "EXIT_CODE=%ERRORLEVEL%"
if not "%EXIT_CODE%"=="0" (
    echo.
    echo 换机包未创建。请根据上面的提示处理后重试。
)
pause
exit /b %EXIT_CODE%
