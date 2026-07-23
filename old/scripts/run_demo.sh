#!/usr/bin/env sh
# printer-fw をクローンした環境で、ビルドしてデモ/テストを実行しログを表示する。
#   Ubuntu 端末、または TeraTerm（シリアル/SSH）から実行すると動作ログが見える。
set -e
cd "$(dirname "$0")/.."

echo "=================================================="
echo " printer-fw build & run"
echo "=================================================="

if command -v cmake >/dev/null 2>&1; then
    echo "[1/3] configure (cmake)"
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    echo "[2/3] build"
    cmake --build build
    DEMO=./build/app_demo
    TESTS=./build/pf_tests
else
    echo "cmake が無いため Makefile でビルドします"
    make
    DEMO=./build/app_demo
    TESTS=./build/pf_tests
fi

echo "[3/3] run tests"
"$TESTS" || echo "(tests reported failures)"

echo
echo "=================================================="
echo " app_demo（状態変化ログ / [I]=info [W]=warn）"
echo "=================================================="
"$DEMO"
