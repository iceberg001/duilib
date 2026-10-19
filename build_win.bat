@echo off
setlocal EnableDelayedExpansion
chcp 65001 >nul 2>&1

rem ============================================================================
rem  directui Windows 平台编译脚本 (MSVC)
rem
rem  用法:
rem    build_win.bat              默认 Release 构建
rem    build_win.bat debug        Debug 构建
rem    build_win.bat release      Release 构建
rem    build_win.bat clean        清理构建目录
rem
rem  自动查找 Visual Studio 2019/2022, 用 cmake 生成 VS 工程并编译。
rem ============================================================================

set "SCRIPT_DIR=%~dp0"
rem 去掉末尾反斜杠, 避免 cmake 命令行中 "path\" 的 \" 被当作转义引号
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
cd /d "%SCRIPT_DIR%"

set "BUILD_DIR=%SCRIPT_DIR%\build_win"
set "BUILD_TYPE=Release"

if /i "%~1"=="debug"   set "BUILD_TYPE=Debug"
if /i "%~1"=="release" set "BUILD_TYPE=Release"
if /i "%~1"=="clean"   (
    echo [INFO] Clean: %BUILD_DIR%
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
    echo [OK] Clean done
    goto :eof
)

echo ============================================================
echo  directui Windows Build
echo  Type : %BUILD_TYPE%
echo  Dir  : %BUILD_DIR%
echo ============================================================

where cmake >nul 2>&1
if errorlevel 1 (
    echo [ERROR] cmake not found in PATH
    goto :fail
)
echo [OK] cmake found

rem ----------------------------------------------------------------------------
rem 查找 VS: 优先 VS2022 Community, 然后其他 edition/版本
rem ----------------------------------------------------------------------------
set "VCVARS_BAT="
set "VS_VER="

call :find_vs 2022 Community      2022
call :find_vs 2022 Professional   2022
call :find_vs 2022 Enterprise     2022
call :find_vs 2022 BuildTools     2022
call :find_vs 2019 Community      2019
call :find_vs 2019 Professional   2019
call :find_vs 2019 Enterprise     2019
call :find_vs 2019 BuildTools     2019

if not defined VCVARS_BAT (
    echo [ERROR] No Visual Studio 2019/2022 with VC++ workload found
    goto :fail
)
echo [OK] MSVC: !VCVARS_BAT!  (VS!VS_VER!)

rem ----------------------------------------------------------------------------
rem 初始化 MSVC 环境
rem ----------------------------------------------------------------------------
echo [INFO] Init MSVC env...
call "!VCVARS_BAT!" >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Init MSVC env failed
    goto :fail
)
echo [OK] MSVC env ready

rem ----------------------------------------------------------------------------
rem 准备构建目录
rem ----------------------------------------------------------------------------
if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
mkdir "%BUILD_DIR%"

rem ----------------------------------------------------------------------------
rem CMake 配置 + 编译 (直接硬编码避免引号传递问题)
rem ----------------------------------------------------------------------------
echo [INFO] CMake configure...

if "!VS_VER!"=="2022" (
    cmake -S "%SCRIPT_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
) else (
    cmake -S "%SCRIPT_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
)
if errorlevel 1 (
    echo [ERROR] CMake configure failed
    goto :fail
)
echo [OK] CMake configure done

echo [INFO] Building (parallel 8)...
cmake --build "%BUILD_DIR%" --config %BUILD_TYPE% -- /m:8
if errorlevel 1 (
    echo [ERROR] Build failed
    goto :fail
)
echo [OK] Build complete

rem ----------------------------------------------------------------------------
rem 结果汇报
rem ----------------------------------------------------------------------------
echo.
echo ============================================================
echo  Build Succeeded!
echo ============================================================
echo  Output: %BUILD_DIR%\Release
echo.
if exist "%BUILD_DIR%\Release\DuiLib.lib" echo  [lib]  DuiLib.lib
for %%F in (360SafeDemo GameDemo ListDemo RichEditDemo RichListDemo NodeDemo) do (
    if exist "%BUILD_DIR%\Release\%%F.exe" echo  [exe]  %%F.exe
)
echo ============================================================
goto :eof

rem ============================================================================
rem :find_vs  year  edition  vs_ver
rem ============================================================================
:find_vs
if not defined VCVARS_BAT (
    set "CAND=%ProgramFiles%\Microsoft Visual Studio\%~1\%~2\VC\Auxiliary\Build\vcvars64.bat"
    if exist "!CAND!" (
        set "VCVARS_BAT=!CAND!"
        set "VS_VER=%~3"
    )
)
if not defined VCVARS_BAT (
    set "CAND=%ProgramFiles(x86)%\Microsoft Visual Studio\%~1\%~2\VC\Auxiliary\Build\vcvars64.bat"
    if exist "!CAND!" (
        set "VCVARS_BAT=!CAND!"
        set "VS_VER=%~3"
    )
)
goto :eof

:fail
echo.
echo ============================================================
echo  Build FAILED!
echo ============================================================
exit /b 1
