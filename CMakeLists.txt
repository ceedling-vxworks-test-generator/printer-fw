cmake_minimum_required(VERSION 3.15)
project(printer_fw C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra)
endif()

# --- コアライブラリ（共通・機種非依存）---
add_library(printer_fw STATIC
    src/pf_core.c
    src/pf_data.c
    src/pf_state.c
    src/pf_fsm.c
    src/pf_monitor.c
    src/pf_observer.c
    src/pf_result.c
)
target_include_directories(printer_fw
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# --- サンプル port（機種/環境依存の参考実装）---
add_library(printer_fw_port_sample STATIC
    port/pf_port_baremetal.c
    port/pf_port_freertos.c
)
target_include_directories(printer_fw_port_sample
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/port
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# --- サンプル機種（機種依存の参考実装）---
add_library(printer_fw_model_sample STATIC
    models/model_sample/model_sample.c
)
target_include_directories(printer_fw_model_sample
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/models/model_sample
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(printer_fw_model_sample PUBLIC printer_fw)

# --- デモ実行ファイル ---
add_executable(app_demo examples/app_demo.c)
target_link_libraries(app_demo PRIVATE
    printer_fw printer_fw_model_sample printer_fw_port_sample)

# --- ユニットテスト ---
enable_testing()
add_executable(pf_tests tests/test_printer_fw.c)
target_link_libraries(pf_tests PRIVATE
    printer_fw printer_fw_model_sample printer_fw_port_sample)
add_test(NAME pf_tests COMMAND pf_tests)
