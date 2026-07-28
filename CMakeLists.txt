if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

cmake_minimum_required(VERSION 3.20)

project(RIManager_Impl VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# mainFW へ add_subdirectory() で組み込まれた場合(=このプロジェクトがトップレベルでない場合)は、
# 既定でテストをビルドしない。rimanager 自身の gtest/FetchContent がmainFW側のビルドを
# 巻き込まないようにするため(ネットワーク制限下のCIでも rimanager 本体は問題なく組み込める)。
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(RIMANAGER_IS_TOP_LEVEL ON)
else()
    set(RIMANAGER_IS_TOP_LEVEL OFF)
endif()
option(RIMANAGER_BUILD_TESTS "Build rimanager's own unit/integration tests" ${RIMANAGER_IS_TOP_LEVEL})

#
# インクルードルート:
#   framework/inc          … 機種共通(全レイヤ機構・共通型・抽象IF・固定容量コンテナ)
#   devices/printer_a/inc  … PrinterA 固有(DataProfile / Rule / CapabilityBuilder /
#                             datastore/RIMDataId.hpp = id一覧 / capi/rim_ids.h = idのCミラー)
#   capi/inc                … C言語のmainFWから呼ぶための唯一の公開ヘッダ(rim_capi.h)。
#                             内部実装はC++のまま、extern "C" な薄いシムを被せる。
# 各ルート配下でレイヤー接頭辞を維持しているため既存 #include はそのまま解決される。
# 特に datastore/RIMDataId.hpp / capi/rim_ids.h は framework 側には存在せず devices 側のみが
# 提供する(framework は型名にのみ依存し、id の中身を知らない。機種展開時はこれらのヘッダを
# devices/<機種>/inc/ に用意すれば framework は無改修のまま差し替わる)。
# devices/_skeleton は雛形なのでビルド対象に含めない。
# install 後は全ルートを1つの include/ 以下へ統合するため、mainFW 側の #include は
# ビルド前後で書き換え不要(下記 install(DIRECTORY ...) 参照)。
#
set(RIM_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/framework/inc
    ${PROJECT_SOURCE_DIR}/devices/printer_a/inc
    ${PROJECT_SOURCE_DIR}/capi/inc
)

add_library(rimanager STATIC)
add_library(rimanager::rimanager ALIAS rimanager)

target_include_directories(rimanager
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/framework/inc>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/devices/printer_a/inc>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/capi/inc>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

file(GLOB_RECURSE RIM_SOURCES
    CONFIGURE_DEPENDS
    framework/src/*.cpp
    devices/printer_a/src/*.cpp
    capi/src/*.cpp
)

target_sources(rimanager
    PRIVATE
        ${RIM_SOURCES}
)

# ============================================================
# インストール / エクスポート(mainFW への組み込み・find_package 用)
# ============================================================
install(TARGETS rimanager
    EXPORT  rimanagerTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 公開ヘッダ: framework(機種共通)と devices/printer_a(機種固有)を同じ include/ 直下へ
# マージしてインストールする(パス上の衝突なし。datastore/RIMDataId.hpp は devices 側にしか
# 存在しないため、framework 側の同名ディレクトリへ自然に合流する)。
install(DIRECTORY ${PROJECT_SOURCE_DIR}/framework/inc/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/devices/printer_a/inc/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/capi/inc/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT rimanagerTargets
    FILE      rimanagerTargets.cmake
    NAMESPACE rimanager::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rimanager
)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/rimanagerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/rimanagerConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rimanager
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/rimanagerConfigVersion.cmake
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/rimanagerConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/rimanagerConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rimanager
)

# pkg-config(find_package を使わない mainFW ビルド向け)
configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/rimanager.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/rimanager.pc
    @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rimanager.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# ============================================================
# テスト(rimanager_test) — RIMANAGER_BUILD_TESTS=ON の時のみ
# ============================================================
if(RIMANAGER_BUILD_TESTS)
    include(CTest)
    enable_testing()

    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_MakeAvailable(googletest)

    file(GLOB_RECURSE TEST_SOURCES
        CONFIGURE_DEPENDS
        test/**/*.cpp
    )

    add_executable(rimanager_test
        ${TEST_SOURCES}
    )

    target_include_directories(rimanager_test
        PRIVATE
            ${PROJECT_SOURCE_DIR}
            ${RIM_INCLUDE_DIRS}
    )

    target_link_libraries(rimanager_test
        PRIVATE
            rimanager
            GTest::gtest
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(rimanager_test)

    # rim_capi.h が実際にC言語から呼べることを検証するスモークテスト(gtest不使用、Cコンパイラでビルド)。
    add_executable(rim_capi_smoke capi/test/rim_capi_smoke.c)
    target_include_directories(rim_capi_smoke PRIVATE ${PROJECT_SOURCE_DIR}/capi/inc ${PROJECT_SOURCE_DIR}/devices/printer_a/inc)
    target_link_libraries(rim_capi_smoke PRIVATE rimanager)
    # 唯一のソースはCだが、rimanager(C++実装)とリンクするため C++ ランタイムでリンクする。
    set_target_properties(rim_capi_smoke PROPERTIES LINKER_LANGUAGE CXX)
    add_test(NAME rim_capi_smoke COMMAND rim_capi_smoke)
endif()
