if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

cmake_minimum_required(VERSION 3.20)

project(
    RIManager_Impl
    LANGUAGES CXX)

option(
    ENABLE_COVERAGE
    "Enable coverage reporting"
    OFF)

#
# Common compile/link options
#

set(
    RIM_COMPILE_OPTIONS

    -fno-exceptions
    -fno-rtti
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
)

set(
    RIM_LINK_OPTIONS
)

#
# Footprint optimization
#
# Coverage時は無効
#

if(NOT ENABLE_COVERAGE)

    list(APPEND
        RIM_COMPILE_OPTIONS

        -ffunction-sections
        -fdata-sections)

    list(APPEND
        RIM_LINK_OPTIONS

        -Wl,--gc-sections)
endif()

if(ENABLE_COVERAGE
   AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    add_compile_options(
        -O0
        -g
        --coverage
        -fprofile-update=atomic)

    add_link_options(
        --coverage)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# STATIC Library / SHARED Library
#

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(
    core)

add_subdirectory(
    products/printer_a)

include(CTest)
enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(
    googletest)

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

        ${PROJECT_SOURCE_DIR}/include

        ${PROJECT_SOURCE_DIR}/core/common/include
        ${PROJECT_SOURCE_DIR}/core/adapter/include
        ${PROJECT_SOURCE_DIR}/core/store/include
        ${PROJECT_SOURCE_DIR}/core/storage/include
        ${PROJECT_SOURCE_DIR}/core/capability/include
        ${PROJECT_SOURCE_DIR}/core/publisher/include

        ${PROJECT_SOURCE_DIR}/products/printer_a
)

target_include_directories(
    rimanager_performance_test
    PRIVATE
        ${PROJECT_SOURCE_DIR}

        ${PROJECT_SOURCE_DIR}/include

        ${PROJECT_SOURCE_DIR}/core/common/include
        ${PROJECT_SOURCE_DIR}/core/adapter/include
        ${PROJECT_SOURCE_DIR}/core/store/include
        ${PROJECT_SOURCE_DIR}/core/storage/include
        ${PROJECT_SOURCE_DIR}/core/capability/include
        ${PROJECT_SOURCE_DIR}/core/publisher/include

        ${PROJECT_SOURCE_DIR}/products/printer_a
)

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

target_link_options(
    rimanager_functional_test
    PRIVATE
        ${RIM_LINK_OPTIONS}
)

target_link_options(
    rimanager_performance_test
    PRIVATE
        ${RIM_LINK_OPTIONS}
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
            --ignore-errors mismatch,negative,gcov
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
