#!/bin/bash
# ============================================================================
#  directui Linux 平台编译脚本
#
#  用法:
#    ./build_linux.sh              默认 Release + X11 后端
#    ./build_linux.sh debug        Debug 构建
#    ./build_linux.sh release      Release 构建
#    ./build_linux.sh gtk          使用 GTK 后端 (Release)
#    ./build_linux.sh gtk debug    使用 GTK 后端 + Debug
#    ./build_linux.sh clean        清理构建目录
#
#  脚本会自动检查依赖, 使用 cmake + make 编译 DuiLib 动态库及所有 demo。
# ============================================================================

set -e

# 切换到脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

BUILD_DIR="$SCRIPT_DIR/build_linux"
BUILD_TYPE="Release"
USE_GTK="OFF"

# 解析命令行参数
for arg in "$@"; do
    case "$arg" in
        debug)   BUILD_TYPE="Debug" ;;
        release) BUILD_TYPE="Release" ;;
        gtk)     USE_GTK="ON" ;;
        clean)
            echo "[INFO] Clean: $BUILD_DIR"
            rm -rf "$BUILD_DIR"
            echo "[OK] Clean done"
            exit 0
            ;;
        *)
            echo "[WARN] Unknown arg: $arg"
            ;;
    esac
done

BACKEND_NAME="X11"
[ "$USE_GTK" = "ON" ] && BACKEND_NAME="GTK"

echo "============================================================"
echo "  directui Linux Build"
echo "  Type   : $BUILD_TYPE"
echo "  Backend: $BACKEND_NAME"
echo "  Dir    : $BUILD_DIR"
echo "============================================================"

# ----------------------------------------------------------------------------
# 1. 检查 cmake
# ----------------------------------------------------------------------------
if ! command -v cmake >/dev/null 2>&1; then
    echo "[ERROR] cmake not found"
    echo "        Install: sudo apt install cmake  (or yum/dnf equivalent)"
    exit 1
fi
echo "[OK] cmake: $(cmake --version | head -1)"

# ----------------------------------------------------------------------------
# 2. 检查编译器
# ----------------------------------------------------------------------------
if ! command -v g++ >/dev/null 2>&1; then
    echo "[ERROR] g++ not found"
    echo "        Install: sudo apt install g++  (or gcc-c++)"
    exit 1
fi
echo "[OK] g++: $(g++ --version | head -1)"

# ----------------------------------------------------------------------------
# 3. 检查 pkg-config
# ----------------------------------------------------------------------------
if ! command -v pkg-config >/dev/null 2>&1; then
    echo "[ERROR] pkg-config not found"
    echo "        Install: sudo apt install pkg-config"
    exit 1
fi
echo "[OK] pkg-config found"

# ----------------------------------------------------------------------------
# 4. 检查后端依赖
# ----------------------------------------------------------------------------
if [ "$USE_GTK" = "ON" ]; then
    echo "[INFO] Checking GTK3 dependencies..."
    if ! pkg-config --exists gtk+-3.0; then
        echo "[ERROR] gtk+-3.0 not found"
        echo "        Install: sudo apt install libgtk-3-dev"
        exit 1
    fi
    echo "[OK] gtk+-3.0: $(pkg-config --modversion gtk+-3.0)"
else
    echo "[INFO] Checking X11/Pango dependencies..."
    MISS=""
    for lib in pango pangoxft freetype2; do
        if ! pkg-config --exists "$lib"; then
            MISS="$MISS $lib"
        fi
    done
    if [ -n "$MISS" ]; then
        echo "[ERROR] Missing:$MISS"
        echo "        Install: sudo apt install libpango1.0-dev libpangoxft-1.0-dev libfreetype6-dev"
        exit 1
    fi
    echo "[OK] pango: $(pkg-config --modversion pango)"
    echo "[OK] freetype2: $(pkg-config --modversion freetype2)"
    # X11 库通过 -lX11 等链接, 检查头文件
    if [ ! -f /usr/include/X11/Xlib.h ] && [ ! -f /usr/local/include/X11/Xlib.h ]; then
        echo "[ERROR] X11 headers not found"
        echo "        Install: sudo apt install libx11-dev libxext-dev libxft-dev libxrender-dev"
        exit 1
    fi
    echo "[OK] X11 headers found"
fi

# ----------------------------------------------------------------------------
# 5. CMake 配置
# ----------------------------------------------------------------------------
echo "[INFO] CMake configure..."
mkdir -p "$BUILD_DIR"

cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DGTK_BACKEND="$USE_GTK"

if [ $? -ne 0 ]; then
    echo "[ERROR] CMake configure failed"
    exit 1
fi
echo "[OK] CMake configure done"

# ----------------------------------------------------------------------------
# 6. 编译 (并行)
# ----------------------------------------------------------------------------
JOBS=$(nproc 2>/dev/null || echo 4)
echo "[INFO] Building with $JOBS jobs..."

cmake --build "$BUILD_DIR" -- -j"$JOBS"
if [ $? -ne 0 ]; then
    echo "[ERROR] Build failed"
    exit 1
fi
echo "[OK] Build complete"

# ----------------------------------------------------------------------------
# 7. 结果汇报
# ----------------------------------------------------------------------------
echo ""
echo "============================================================"
echo "  Build Succeeded!"
echo "============================================================"
echo "  Output: $BUILD_DIR"
echo ""
if [ -f "$BUILD_DIR/libDuiLib.so" ]; then
    echo "  [lib]  libDuiLib.so"
fi
for exe in 360SafeDemo GameDemo ListDemo RichEditDemo RichListDemo; do
    if [ -f "$BUILD_DIR/$exe" ]; then
        echo "  [exe]  $exe"
    fi
done
echo ""
echo "  Run: cd \"$BUILD_DIR\" && ./360SafeDemo"
echo "  (If 'libDuiLib.so not found', run: export LD_LIBRARY_PATH=\"$BUILD_DIR:\$LD_LIBRARY_PATH\")"
echo "============================================================"