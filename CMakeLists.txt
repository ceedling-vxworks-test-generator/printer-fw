if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

cmake_minimum_required(VERSION 3.20)

project(RIManager_Impl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# インクルードルート:
#   framework/inc          … 機種共通(全レイヤ機構・共通型・抽象IF・固定容量コンテナ)
#   devices/printer_a/inc  … PrinterA 固有(DataProfile / Rule / CapabilityBuilder /
#                             datastore/RIMDataId.hpp = id一覧)
# 各ルート配下でレイヤー接頭辞を維持しているため既存 #include はそのまま解決される。
# 特に datastore/RIMDataId.hpp は framework 側には存在せず devices 側のみが提供する
# (framework は型名にのみ依存し、id の中身を知らない。機種展開時はこのヘッダを
# devices/<機種>/inc/datastore/ に用意すれば framework は無改修のまま差し替わる)。
# devices/_skeleton は雛形なのでビルド対象に含めない。
#
set(RIM_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/framework/inc
    ${PROJECT_SOURCE_DIR}/devices/printer_a/inc
)

add_library(rimanager STATIC)

target_include_directories(rimanager
    PUBLIC
        ${RIM_INCLUDE_DIRS}
)

file(GLOB_RECURSE RIM_SOURCES
    CONFIGURE_DEPENDS
    framework/src/*.cpp
    devices/printer_a/src/*.cpp
)

target_sources(rimanager
    PRIVATE
        ${RIM_SOURCES}
)

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
