@echo off
setlocal EnableDelayedExpansion
chcp 65001 >nul 2>&1

rem ============================================================================
rem  directui WSL Linux 交叉编译脚本
rem
rem  在 Windows 上调用 WSL 编译 Linux 版本, 产物在 build_linux_output/
rem  编译后可直接将 build_linux_output/ 拷贝到任意 Linux 机器运行
rem
rem  用法:
rem    build_linux_wsl.bat              默认 X11 Release
rem    build_linux_wsl.bat gtk          GTK 后端
rem    build_linux_wsl.bat debug        Debug 模式
rem    build_linux_wsl.bat gtk debug    GTK + Debug
rem
rem  前提: 已安装 WSL + Ubuntu (wsl --install -d Ubuntu)
rem ============================================================================

set "SCRIPT_DIR=%~dp0"
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
cd /d "%SCRIPT_DIR%"

echo ============================================================
echo  directui WSL Linux Build
echo  Source: %SCRIPT_DIR%
echo ============================================================

rem ----------------------------------------------------------------------------
rem 1. 检查 WSL
rem ----------------------------------------------------------------------------
wsl --list --quiet >nul 2>&1
if errorlevel 1 (
    echo [ERROR] WSL not installed or no distribution installed
    echo.
    echo Please install WSL Ubuntu first:
    echo   wsl --install -d Ubuntu
    echo.
    echo Or if network issues, download from:
    echo   https://ubuntu.com/wsl
    goto :fail
)
echo [OK] WSL available

rem ----------------------------------------------------------------------------
rem 2. 获取盘符, 构造 WSL 路径
rem    I:\跨平台\directui -> /mnt/i/跨平台/directui
rem ----------------------------------------------------------------------------
set "DRIVE=%SCRIPT_DIR:~0,1%"
set "DRIVE_LOWER=!DRIVE!"
rem 转小写
for %%a in (a b c d e f g h i j k l m n o p q r s t u v w x y z) do (
    if /i "!DRIVE!"=="%%a" set "DRIVE_LOWER=%%a"
)

set "PATH_AFTER_DRIVE=%SCRIPT_DIR:~2%"
rem 替换 \ 为 /
set "WSL_PATH=!PATH_AFTER_DRIVE:\=/!"

set "WSL_FULL_PATH=/mnt/!DRIVE_LOWER!!WSL_PATH!"

echo [OK] WSL path: !WSL_FULL_PATH!

rem ----------------------------------------------------------------------------
rem 3. 在 WSL 中执行编译
rem    传递参数给 build_linux_on_wsl.sh
rem ----------------------------------------------------------------------------
set "WSL_ARGS=%~1 %~2 %~3"
if "!WSL_ARGS!"=="  " set "WSL_ARGS="

echo [INFO] Starting build in WSL...
echo.

wsl bash -c "cd '!WSL_FULL_PATH!' && bash build_linux_on_wsl.sh !WSL_ARGS!"
if errorlevel 1 (
    echo.
    echo [ERROR] WSL build failed
    goto :fail
)

echo.
echo ============================================================
echo  WSL Build Succeeded!
echo ============================================================
echo  Output: %SCRIPT_DIR%\build_linux_output\
echo.
echo  Deploy to Linux:
echo    1. Copy build_linux_output\ to target Linux machine
echo    2. On target: cd build_linux_output ^&^& ./run_demo.sh
echo ============================================================
goto :eof

:fail
echo.
echo ============================================================
echo  Build FAILED!
echo ============================================================
exit /b 1