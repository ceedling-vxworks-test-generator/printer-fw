cmake_minimum_required(VERSION 3.15)
project(printer_fw VERSION 1.0.0 LANGUAGES C)

# ---- オプション（ライブラリ利用者は EXAMPLES/TESTS を OFF にできる）----
option(PRINTER_FW_BUILD_EXAMPLES     "Build the demo executable"       ON)
option(PRINTER_FW_BUILD_TESTS        "Build the unit tests"            ON)
option(PRINTER_FW_BUILD_SAMPLES      "Build sample port/model libs"    ON)
# 共有/静的の切替は標準の BUILD_SHARED_LIBS を使用（既定は静的）。
#   例: cmake -S . -B build -DBUILD_SHARED_LIBS=ON

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ============================================================
# コアライブラリ（共通・機種非依存） ＝ インストール対象
# ============================================================
add_library(printer_fw
    src/pf_core.c
    src/pf_data.c
    src/pf_state.c
    src/pf_fsm.c
    src/pf_monitor.c
    src/pf_observer.c
    src/pf_result.c
    src/pf_log.c
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
    target_compile_options(printer_fw PRIVATE /W3)
else()
    target_compile_options(printer_fw PRIVATE -Wall -Wextra)
endif()

# ============================================================
# サンプル port / model（参考実装。既定でビルドするが install しない）
# ============================================================
if(PRINTER_FW_BUILD_SAMPLES)
    add_library(printer_fw_port_sample STATIC
        port/pf_port_baremetal.c
        port/pf_port_freertos.c
    )
    target_include_directories(printer_fw_port_sample
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/port
    )
    target_link_libraries(printer_fw_port_sample PUBLIC printer_fw)

    # Linux(POSIX) 専用 port は UNIX のときだけ追加し、pthread をリンク
    if(UNIX AND NOT APPLE)
        find_package(Threads REQUIRED)
        target_sources(printer_fw_port_sample PRIVATE port/pf_port_linux.c)
        target_link_libraries(printer_fw_port_sample PUBLIC Threads::Threads)
    endif()

    add_library(printer_fw_model_sample STATIC
        models/model_sample/model_sample.c
    )
    target_include_directories(printer_fw_model_sample
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/models/model_sample
    )
    target_link_libraries(printer_fw_model_sample PUBLIC printer_fw)
endif()

# ============================================================
# デモ / テスト
# ============================================================
if(PRINTER_FW_BUILD_EXAMPLES AND PRINTER_FW_BUILD_SAMPLES)
    add_executable(app_demo examples/app_demo.c)
    target_link_libraries(app_demo PRIVATE
        printer_fw printer_fw_model_sample printer_fw_port_sample)
endif()

if(PRINTER_FW_BUILD_TESTS AND PRINTER_FW_BUILD_SAMPLES)
    enable_testing()
    add_executable(pf_tests tests/test_printer_fw.c)
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

# 公開ヘッダ一式
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
