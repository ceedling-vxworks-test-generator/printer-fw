#
# mainfw-mock - printer-fw(librimanager.so)を外部から消費する最小のCプロジェクト。
#
# 運用方針: printer-fwを自動ビルド・自動installしない。
# 人がビルド済みの成果物を、このディレクトリ直下へ手で配置する運用にする
# (mainFW側リポジトリがベンダーライブラリを同梱する一般的な構成を模している)。
#
#   lib/librimanager.so   <- 配置必須。無ければ「リンクエラー」でビルドが失敗する
#   include/*             <- 配置必須。無ければ「コンパイルエラー」でビルドが失敗する
#
# 配置手順(printer-fw側をビルドし、ここへコピーする):
#
#   cd ..
#   cmake -S . -B build-shared -DCMAKE_BUILD_TYPE=Debug -DRIMANAGER_BUILD_TESTS=OFF
#   cmake --build build-shared --target rimanager
#   cmake --install build-shared --prefix "$(pwd)/mainfw-mock"
#
# ↑ この手順は「lib/・include/を用意する側の作業」であり、mainfw-mock自身の
#   configure/buildからは一切呼び出さない(意図的に自動化しない)。
#
# 配置後、このディレクトリで:
#
#   cmake -S . -B build
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

# configure時点では存在チェックだけ行い、警告に留める(configureは通す)。
# 実際に「置いていなければ動かない」を体現するのはコンパイル/リンクの段階。
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/rim_api.h")
    message(WARNING
        "include/rim_api.h が見つかりません。"
        "printer-fwをビルドし、include/ へヘッダ一式を配置してください"
        "(配置手順はCMakeLists.txt冒頭のコメント参照)。"
        "このままビルドするとコンパイルエラーになります。")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/librimanager.so")
    message(WARNING
        "lib/librimanager.so が見つかりません。"
        "printer-fwをビルドし、lib/ へ配置してください"
        "(配置手順はCMakeLists.txt冒頭のコメント参照)。"
        "このままビルドするとリンクエラーになります。")
endif()

add_executable(mainfw_mock src/main.c)

target_include_directories(mainfw_mock PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

# find_package は使わない(=CMake設定一式が無くてもよい)。
# lib/ をリンク検索パスに追加し、名前(-lrimanager)でリンクする。
# lib/librimanager.so が無ければ、この時点でリンカが
# 「cannot find -lrimanager」というリンクエラーを出す(意図した挙動)。
target_link_directories(mainfw_mock PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib)

target_link_libraries(mainfw_mock PRIVATE rimanager)

#
# 実行時にlibrimanager.soを見つけられるよう、ビルド木のRPATHへlib/を埋め込む
# (LD_LIBRARY_PATHを手で通さなくても動かせるように)。
#
set_target_properties(mainfw_mock PROPERTIES
    BUILD_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)
