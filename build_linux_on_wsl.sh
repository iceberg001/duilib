#!/bin/bash
# ============================================================================
#  directui Linux 编译脚本 (在 WSL 或原生 Linux 中运行)
#
#  功能:
#    1. 自动安装编译依赖 (cmake/g++/gtk3/pango/freetype2/X11)
#    2. 编译 DuiLib 动态库 + 所有 demo (含 NodeDemo)
#    3. 产物收集到 build_linux_output/ 便于拷贝到其他 Linux 机器
#
#  用法 (在 WSL 中):
#    cd /mnt/i/跨平台/directui
#    bash build_linux_on_wsl.sh           # 默认 X11 后端 Release
#    bash build_linux_on_wsl.sh gtk       # GTK 后端
#    bash build_linux_on_wsl.sh debug     # Debug 模式
# ============================================================================

set -e

# 获取脚本所在目录作为项目根目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

BUILD_DIR="$SCRIPT_DIR/build_linux"
OUTPUT_DIR="$SCRIPT_DIR/build_linux_output"
BUILD_TYPE="Release"
USE_GTK="OFF"

for arg in "$@"; do
    case "$arg" in
        debug)   BUILD_TYPE="Debug" ;;
        release) BUILD_TYPE="Release" ;;
        gtk)     USE_GTK="ON" ;;
    esac
done

BACKEND_NAME="X11"
[ "$USE_GTK" = "ON" ] && BACKEND_NAME="GTK"

echo "============================================================"
echo "  directui Linux Build (WSL)"
echo "  Type   : $BUILD_TYPE"
echo "  Backend: $BACKEND_NAME"
echo "  Source : $SCRIPT_DIR"
echo "============================================================"

# ----------------------------------------------------------------------------
# 1. 检测包管理器并安装依赖
# ----------------------------------------------------------------------------
install_deps() {
    echo "[INFO] Checking and installing dependencies..."

    if command -v apt-get >/dev/null 2>&1; then
        PKGS="cmake g++ pkg-config"
        if [ "$USE_GTK" = "ON" ]; then
            PKGS="$PKGS libgtk-3-dev"
        else
            PKGS="$PKGS libpango1.0-dev libpangoxft-1.0-dev libfreetype6-dev"
            PKGS="$PKGS libx11-dev libxext-dev libxft-dev libxrender-dev"
        fi

        # 检查哪些包需要安装
        NEED_INSTALL=""
        for pkg in $PKGS; do
            if ! dpkg -s "$pkg" >/dev/null 2>&1; then
                NEED_INSTALL="$NEED_INSTALL $pkg"
            fi
        done

        if [ -n "$NEED_INSTALL" ]; then
            echo "[INFO] Installing:$NEED_INSTALL"
            sudo apt-get update -qq
            sudo apt-get install -y -qq $NEED_INSTALL
        else
            echo "[OK] All dependencies already installed"
        fi
    elif command -v yum >/dev/null 2>&1; then
        PKGS="cmake gcc-c++ pkgconfig"
        if [ "$USE_GTK" = "ON" ]; then
            PKGS="$PKGS gtk3-devel"
        else
            PKGS="$PKGS pango-devel freetype-devel libX11-devel libXext-devel libXft-devel libXrender-devel"
        fi
        echo "[INFO] Installing: $PKGS"
        sudo yum install -y -q $PKGS
    elif command -v dnf >/dev/null 2>&1; then
        PKGS="cmake gcc-c++ pkgconfig"
        if [ "$USE_GTK" = "ON" ]; then
            PKGS="$PKGS gtk3-devel"
        else
            PKGS="$PKGS pango-devel freetype-devel libX11-devel libXext-devel libXft-devel libXrender-devel"
        fi
        echo "[INFO] Installing: $PKGS"
        sudo dnf install -y -q $PKGS
    else
        echo "[WARN] Unknown package manager, skipping dependency installation"
        echo "       Please install cmake, g++, and GUI dev libraries manually"
    fi
}

install_deps

# ----------------------------------------------------------------------------
# 2. 验证工具链
# ----------------------------------------------------------------------------
echo "[INFO] Verifying toolchain..."
cmake --version | head -1
g++ --version | head -1

if ! command -v cmake >/dev/null 2>&1; then
    echo "[ERROR] cmake not found"
    exit 1
fi
if ! command -v g++ >/dev/null 2>&1; then
    echo "[ERROR] g++ not found"
    exit 1
fi
echo "[OK] Toolchain ready"

# ----------------------------------------------------------------------------
# 3. CMake 配置 + 编译
# ----------------------------------------------------------------------------
echo "[INFO] CMake configure..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DGTK_BACKEND="$USE_GTK"
if [ $? -ne 0 ]; then
    echo "[ERROR] CMake configure failed"
    exit 1
fi
echo "[OK] CMake configure done"

JOBS=$(nproc 2>/dev/null || echo 4)
echo "[INFO] Building with $JOBS jobs..."
cmake --build "$BUILD_DIR" -- -j"$JOBS"
if [ $? -ne 0 ]; then
    echo "[ERROR] Build failed"
    exit 1
fi
echo "[OK] Build complete"

# ----------------------------------------------------------------------------
# 4. 收集产物到 build_linux_output/
# ----------------------------------------------------------------------------
echo "[INFO] Collecting artifacts..."
rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR"

# 拷贝共享库
if [ -f "$BUILD_DIR/libDuiLib.so" ]; then
    cp "$BUILD_DIR/libDuiLib.so" "$OUTPUT_DIR/"
    echo "  [lib]  libDuiLib.so"
fi

# 拷贝可执行文件
for exe in 360SafeDemo GameDemo ListDemo RichEditDemo RichListDemo NodeDemo; do
    if [ -f "$BUILD_DIR/$exe" ]; then
        cp "$BUILD_DIR/$exe" "$OUTPUT_DIR/"
        echo "  [exe]  $exe"
    fi
done

# 拷贝资源目录
for res in 360SafeRes GameRes ListRes RichEditRes NodeDemoRes; do
    if [ -d "$SCRIPT_DIR/demo/"*/"$res" ]; then
        cp -r "$SCRIPT_DIR/demo/"*/"$res" "$OUTPUT_DIR/" 2>/dev/null || true
    fi
done

# 拷贝 RichListRes.zip 和 red_chamber.txt
for f in RichListRes.zip red_chamber.txt; do
    found=$(find "$SCRIPT_DIR/demo" -name "$f" -type f 2>/dev/null | head -1)
    if [ -n "$found" ]; then
        cp "$found" "$OUTPUT_DIR/" 2>/dev/null || true
    fi
done

# 生成运行脚本
cat > "$OUTPUT_DIR/run_demo.sh" << 'RUNEOF'
#!/bin/bash
# 设置库搜索路径为当前目录
export LD_LIBRARY_PATH="$(pwd):$LD_LIBRARY_PATH"
echo "Available demos:"
for f in 360SafeDemo GameDemo ListDemo RichEditDemo RichListDemo NodeDemo; do
    if [ -f "./$f" ]; then
        echo "  ./$f"
    fi
done
echo ""
echo "Usage: ./360SafeDemo  (or other demo name)"
if [ -n "$1" ]; then
    ./$1
fi
RUNEOF
chmod +x "$OUTPUT_DIR/run_demo.sh"

echo ""
echo "============================================================"
echo "  Build Succeeded!"
echo "============================================================"
echo "  Output: $OUTPUT_DIR"
echo ""
ls -la "$OUTPUT_DIR/"
echo ""
echo "  To run on this machine:"
echo "    cd \"$OUTPUT_DIR\" && ./run_demo.sh NodeDemo"
echo ""
echo "  To deploy to another Linux machine:"
echo "    1. Copy entire build_linux_output/ directory"
echo "    2. On target: cd build_linux_output && ./run_demo.sh"
echo "============================================================"