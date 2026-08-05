#
# mainfw-mock - printer-fw(librimanager.so)を外部から消費する最小のCプロジェクト。
#
# 本物のmainFWとは独立したプロジェクトとして、printer-fwを
# find_package(rimanager) 経由で見つけてリンクする(本番の結合方法と同じ経路)。
#
# 事前に printer-fw 側を共有ライブラリとしてビルド・installしておくこと:
#
#   cd ..
#   cmake -S . -B build-shared -DCMAKE_BUILD_TYPE=Debug -DRIMANAGER_BUILD_TESTS=OFF
#   cmake --build build-shared --target rimanager
#   cmake --install build-shared --prefix "$(pwd)/_install"
#
# その後、このディレクトリで:
#
#   cmake -S . -B build -DCMAKE_PREFIX_PATH="$(pwd)/../_install"
#   cmake --build build
#   ./build/mainfw_mock
#
cmake_minimum_required(VERSION 3.20)

project(mainfw_mock LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 既定はDebug(-g付き)。指定を忘れると最適化ビルドになりステップ実行できない
# ため、この単独プロジェクトでは明示的にデフォルトを倒しておく。
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# printer-fw の install prefix を指す。CMAKE_PREFIX_PATH で上書き可能
# (既定は隣の printer-fw/_install を見る)。
if(NOT CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../_install")
endif()

find_package(rimanager REQUIRED)

add_executable(mainfw_mock src/main.c)

target_link_libraries(mainfw_mock PRIVATE rimanager::rimanager)

#
# 実行時にlibrimanager.soを見つけられるよう、ビルド木のRPATHにinstall先の
# libディレクトリを埋め込む(LD_LIBRARY_PATHを手で通さなくても動かせるように)。
# インストールせずビルドディレクトリからそのまま実行するための開発時設定であり、
# 本番配布(mainFW側)のRPATH方針とは別に考えること。
#
set_target_properties(mainfw_mock PROPERTIES
    BUILD_RPATH "${CMAKE_PREFIX_PATH}/lib"
)
