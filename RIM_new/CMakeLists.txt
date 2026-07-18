if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

cmake_minimum_required(VERSION 3.20)

project(RIManager_Impl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# インクルードルート:
#   framework/inc          … 機種共通(パイプライン機構・抽象IF・共通型・Capability構造体)
#   devices/printer_a/inc  … PrinterA 固有(具象 Adapter / Rule / Provider)
#   devices/registry/inc   … 機種選択の配線(AdapterFactory / MachineProfileFactory / MachineProfile)
#
# 各ルート配下でレイヤー接頭辞(adapter/ datastore/ capability/ ...)を維持しているため、
# 既存の #include "<layer>/<file>.hpp" は複数ルート探索でそのまま解決される。
# devices/_skeleton は新機種の雛形なのでビルド対象に含めない。
#
set(RIM_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/framework/inc
    ${PROJECT_SOURCE_DIR}/devices/printer_a/inc
    ${PROJECT_SOURCE_DIR}/devices/registry/inc
)

add_library(rimanager STATIC)

target_include_directories(rimanager
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${RIM_INCLUDE_DIRS}
)

file(GLOB_RECURSE RIM_SOURCES
    CONFIGURE_DEPENDS
    framework/src/*.cpp
    devices/printer_a/src/*.cpp
    devices/registry/src/*.cpp
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
        ${PROJECT_SOURCE_DIR}/include
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
