cmake_minimum_required(VERSION 3.15)
project(printer_fw VERSION 1.0.0 LANGUAGES CXX)

# ---- オプション（ライブラリ利用者は EXAMPLES/TESTS を OFF にできる）----
option(PRINTER_FW_BUILD_EXAMPLES     "Build the demo executable"       ON)
option(PRINTER_FW_BUILD_TESTS        "Build the unit tests"            ON)
option(PRINTER_FW_BUILD_SAMPLES      "Build sample port/model libs"    ON)
# 共有/静的の切替は標準の BUILD_SHARED_LIBS を使用（既定は静的）。
#   例: cmake -S . -B build -DBUILD_SHARED_LIBS=ON

# 実装は組込み向けイディオマティックC++（クラス化・RAII。例外/RTTI/動的確保/STLコンテナ不使用）。
# 公開APIは全て extern "C" で C ABI 互換を維持（既存Cコードからも呼び出し可能）。
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# 組込み制約（例外・RTTI不使用）をビルドでも強制する
if(MSVC)
    set(PRINTER_FW_EMBEDDED_CXX_FLAGS /GR- /EHs-c-)
else()
    set(PRINTER_FW_EMBEDDED_CXX_FLAGS -fno-exceptions -fno-rtti)
endif()

# ============================================================
# コアライブラリ（共通・機種非依存） ＝ インストール対象
# ============================================================
add_library(printer_fw
    src/pf_core.cpp
    src/pf_data.cpp
    src/pf_state.cpp
    src/pf_fsm.cpp
    src/pf_monitor.cpp
    src/pf_observer.cpp
    src/pf_result.cpp
    src/pf_log.cpp
)
add_library(printer_fw::printer_fw ALIAS printer_fw)

target_include_directories(printer_fw
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(printer_fw PROPERTIES
    VERSION   ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(MSVC)
    target_compile_options(printer_fw PRIVATE /W3 ${PRINTER_FW_EMBEDDED_CXX_FLAGS})
else()
    target_compile_options(printer_fw PRIVATE -Wall -Wextra ${PRINTER_FW_EMBEDDED_CXX_FLAGS})
endif()

# ============================================================
# サンプル port / model（参考実装。既定でビルドするが install しない）
# ============================================================
if(PRINTER_FW_BUILD_SAMPLES)
    add_library(printer_fw_port_sample STATIC
        port/pf_port_baremetal.cpp
        port/pf_port_freertos.cpp
    )
    target_include_directories(printer_fw_port_sample
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/port
    )
    target_link_libraries(printer_fw_port_sample PUBLIC printer_fw)
    target_compile_options(printer_fw_port_sample PRIVATE ${PRINTER_FW_EMBEDDED_CXX_FLAGS})

    # Linux(POSIX) 専用 port は UNIX のときだけ追加し、pthread をリンク
    if(UNIX AND NOT APPLE)
        find_package(Threads REQUIRED)
        target_sources(printer_fw_port_sample PRIVATE port/pf_port_linux.cpp)
        target_link_libraries(printer_fw_port_sample PUBLIC Threads::Threads)
    endif()

    add_library(printer_fw_model_sample STATIC
        models/model_sample/model_sample.cpp
    )
    target_include_directories(printer_fw_model_sample
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/models/model_sample
    )
    target_link_libraries(printer_fw_model_sample PUBLIC printer_fw)
    target_compile_options(printer_fw_model_sample PRIVATE ${PRINTER_FW_EMBEDDED_CXX_FLAGS})
endif()

# ============================================================
# デモ / テスト
# ============================================================
if(PRINTER_FW_BUILD_EXAMPLES AND PRINTER_FW_BUILD_SAMPLES)
    add_executable(app_demo examples/app_demo.cpp)
    target_link_libraries(app_demo PRIVATE
        printer_fw printer_fw_model_sample printer_fw_port_sample)
endif()

if(PRINTER_FW_BUILD_TESTS AND PRINTER_FW_BUILD_SAMPLES)
    enable_testing()
    add_executable(pf_tests tests/test_printer_fw.cpp)
    target_link_libraries(pf_tests PRIVATE
        printer_fw printer_fw_model_sample printer_fw_port_sample)
    add_test(NAME pf_tests COMMAND pf_tests)
endif()

# ============================================================
# インストール / エクスポート（ライブラリ化）
# ============================================================
install(TARGETS printer_fw
    EXPORT  printer_fwTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 公開ヘッダ一式（C/C++両方から利用可能な純Cヘッダ。extern "C" でC ABIを提供）
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/printer_fw
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# CMake ターゲットのエクスポート（find_package 用）
install(EXPORT printer_fwTargets
    FILE      printer_fwTargets.cmake
    NAMESPACE printer_fw::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/printer_fw
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/printer_fwConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/printer_fwConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/printer_fw
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/printer_fwConfigVersion.cmake
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/printer_fwConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/printer_fwConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/printer_fw
)

# pkg-config（find_package を使わない場合向け）
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/printer_fw.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/printer_fw.pc
    @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/printer_fw.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
