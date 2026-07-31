if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

cmake_minimum_required(VERSION 3.20)

project(
    RIManager_Impl
    VERSION 0.1.0
    LANGUAGES CXX C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

#
# mainFW へ add_subdirectory() で組み込まれた場合(=このプロジェクトがトップレベルで
# ない場合)は、既定でテストをビルドしない。rimanager 自身の gtest/FetchContent が
# mainFW 側のビルドを巻き込まないようにするため(ネットワーク制限下の CI でも
# rimanager 本体は問題なく組み込める)。
#
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(RIMANAGER_IS_TOP_LEVEL ON)
else()
    set(RIMANAGER_IS_TOP_LEVEL OFF)
endif()

option(RIMANAGER_BUILD_TESTS "Build rimanager's own unit/integration/e2e/performance tests" ${RIMANAGER_IS_TOP_LEVEL})
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -O0
        -g
        --coverage
    )

    add_link_options(
        --coverage
    )
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(
    core)

add_subdirectory(
    products/printer_a)

#
# rim-core 自身の翻訳単位(RIMDataItem.hpp 経由で RIMDataId.hpp を include するもの)を
# コンパイルするには、products 側が提供する RIMDataId.hpp / PrinterARules.hpp が
# 見える必要がある(ヘッダ注入)。
#
# rim-core の CMakeLists.txt にも core の C++ コードにも「printer_a」という文字列は
# 一切現れない。**どの機種向けにビルドするかを知っているのはこの最上位の
# CMakeLists.txt だけ**であり、そこから rim-core のコンパイルにだけ影響する
# PRIVATE パスとして渡す(rim-core の公開 IF には出さない。products 側は自分の
# PUBLIC include で既にこのパスを持っている)。
#
target_include_directories(rim-core
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/products/printer_a/DataItem>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/products/printer_a/Adapter>
)

#
# rimanager - core と products/printer_a のオブジェクトを1つにまとめた実体の
# STATIC ライブラリ。消費側(mainFW)は find_package(rimanager) して
# rimanager::rimanager にリンクすればよい。
#
# rim-core と rim-product-printer-a は互いのシンボルを参照し合う
# (ProductBinding 経由: rim-core の rim_api.cpp が products 側の実装を呼ぶ)。
# 両方を STATIC アーカイブにすると、リンカがアーカイブを1パスしか処理しない
# ため、素直な順序ではどちらを先に置いても他方の未解決シンボルが残る
# (実際に発生することを確認済み)。
#
# core/CMakeLists.txt と products/printer_a/CMakeLists.txt を OBJECT ライブラリに
# しておき、ここで1つの実アーカイブへまとめることで、アーカイブ間の解決順という
# 問題自体を無くしている(1つのアーカイブ内の相互参照はインデックス経由で解決
# できるため、複数アーカイブの時のような1パス制限を受けない)。
#
add_library(rimanager STATIC)
add_library(rimanager::rimanager ALIAS rimanager)

target_link_libraries(rimanager
    PUBLIC
        rim-core
        rim-product-printer-a
)

# ============================================================
# インストール / エクスポート(mainFW への組み込み・find_package 用)
# ============================================================
#
# rim-core / rim-product-printer-a は OBJECT ライブラリであり、それ自体は
# ファイルを生成しない(オブジェクトは rimanager の .a に取り込まれる)。
# それでも rimanager が PUBLIC でリンクしている以上、CMake のターゲットグラフの
# 整合性のため同じ export set に含める必要がある。
#
install(TARGETS rim-core rim-product-printer-a
    EXPORT rimanagerTargets
)
install(TARGETS rimanager
    EXPORT  rimanagerTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

#
# 公開ヘッダ: core の全レイヤと products/printer_a を同じ include/ 直下へ
# マージしてインストールする(ファイル名の衝突が無いことは確認済み)。
# RIMDataId.hpp は products/printer_a/DataItem 側にしか存在しないため、
# core 側の include/ に自然に合流する(ヘッダ注入方式)。
#
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/core/common/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/core/adapter/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/core/store/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/core/storage/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/core/capability/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/core/publisher/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/products/printer_a/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
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
# rim_capi_smoke — C の公開ヘッダ(rim_api.h)が実際に C から呼べることを検証する
# C テスト群。gtest 不使用・ネットワーク不要のため、RIMANAGER_BUILD_TESTS
# (gtest の FetchContent を伴う)とは独立に、トップレベルビルド時のみ既定で
# ビルドする(ネットワーク制限下でもビルド・デバッグ可能)。
# ============================================================
if(RIMANAGER_IS_TOP_LEVEL)
    enable_testing()

    file(GLOB CAPI_TEST_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/test/capi/*.c)

    if(CAPI_TEST_SOURCES)
        add_executable(rim_capi_smoke ${CAPI_TEST_SOURCES})

        target_include_directories(rim_capi_smoke PRIVATE
            ${PROJECT_SOURCE_DIR}/include
            ${PROJECT_SOURCE_DIR}/test/capi
            ${PROJECT_SOURCE_DIR}/products/printer_a)

        target_link_libraries(rim_capi_smoke PRIVATE rimanager)

        # ソースは C だが、rimanager(C++実装)とリンクするため C++ ランタイムでリンクする。
        set_target_properties(rim_capi_smoke PROPERTIES LINKER_LANGUAGE CXX)

        add_test(NAME rim_capi_smoke COMMAND rim_capi_smoke)
    endif()
endif()

# ============================================================
# gtest ベースのテスト(RIMANAGER_BUILD_TESTS=ON の時のみ)
# ============================================================
if(RIMANAGER_BUILD_TESTS)
    include(CTest)
    enable_testing()

    #
    # テストは googletest が例外と RTTI を必要とするため、
    # rim-core から PUBLIC で伝播してくる -fno-exceptions / -fno-rtti を打ち消す。
    # (打ち消すのはテスト実行ファイルだけで、ライブラリ本体の指定は変わらない)
    #
    set(RIM_TEST_ENABLE_EXCEPTIONS
        -fexceptions
        -frtti)

    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_MakeAvailable(googletest)

    file(GLOB_RECURSE FUNCTIONAL_TEST_SOURCES
        CONFIGURE_DEPENDS

        test/unit/*.cpp
        test/unit/**/*.cpp

        test/integration/*.cpp
        test/integration/**/*.cpp

        test/e2e/*.cpp
        test/e2e/**/*.cpp

        test/thread/*.cpp
        test/thread/**/*.cpp
    )

    file(GLOB_RECURSE PERFORMANCE_TEST_SOURCES
        CONFIGURE_DEPENDS

        test/performance/*.cpp
        test/performance/**/*.cpp
    )

    add_executable(
        rimanager_functional_test
        ${FUNCTIONAL_TEST_SOURCES}
    )

    add_executable(
        rimanager_performance_test
        ${PERFORMANCE_TEST_SOURCES}
    )

    target_include_directories(
        rimanager_functional_test
        PRIVATE
            ${PROJECT_SOURCE_DIR}

            # PerformanceTestFixture.hpp / PerformanceStatistics.hpp
            ${PROJECT_SOURCE_DIR}/test/performance
    )

    target_include_directories(
        rimanager_performance_test
        PRIVATE
            ${PROJECT_SOURCE_DIR}

            # PerformanceTestFixture.hpp / PerformanceStatistics.hpp
            ${PROJECT_SOURCE_DIR}/test/performance
    )

    target_compile_options(
        rimanager_functional_test
        PRIVATE
            ${RIM_TEST_ENABLE_EXCEPTIONS})

    target_compile_options(
        rimanager_performance_test
        PRIVATE
            ${RIM_TEST_ENABLE_EXCEPTIONS})

    target_link_libraries(
        rimanager_functional_test
        PRIVATE
            rim-core
            rim-product-printer-a
            GTest::gtest
            GTest::gtest_main
    )

    target_link_libraries(
        rimanager_performance_test
        PRIVATE
            rim-core
            rim-product-printer-a
            GTest::gtest
            GTest::gtest_main
    )

    include(GoogleTest)

    gtest_discover_tests(
        rimanager_functional_test
    )

    gtest_discover_tests(
        rimanager_performance_test
    )

    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)

    if(ENABLE_COVERAGE
       AND LCOV_EXECUTABLE
       AND GENHTML_EXECUTABLE)

        add_custom_target(coverage

            # 古いデータ削除
            COMMAND ${LCOV_EXECUTABLE}
                --directory .
                --zerocounters

            # テスト実行
            COMMAND ${CMAKE_CTEST_COMMAND}
                --output-on-failure

            # カバレッジ収集
            COMMAND ${LCOV_EXECUTABLE}
                --capture
                --ignore-errors mismatch
                --directory .
                --output-file coverage.info

            # 不要コード除外
            COMMAND ${LCOV_EXECUTABLE}
                --remove coverage.info
                '/usr/*'
                '*/_deps/*'
                '*/test/*'
                --output-file coverage.info

            # HTML生成
            COMMAND ${GENHTML_EXECUTABLE}
                coverage.info
                --output-directory coverage_html

            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}

            COMMENT "Generating code coverage report"
        )
    endif()
endif()
