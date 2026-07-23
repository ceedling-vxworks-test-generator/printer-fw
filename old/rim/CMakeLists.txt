cmake_minimum_required(VERSION 3.15)
project(rim LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# RIM 実体（C++・例外/RTTI無効）。公開IFは extern "C"。
add_library(rim STATIC
    src/rim_list.cpp
    src/rim_adapter.cpp
    src/rim_datastore.cpp
    src/rim_capability.cpp
    src/rim_publisher.cpp
    src/rim_core.cpp
    src/rim_port.cpp)
target_include_directories(rim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(rim PRIVATE -fno-exceptions -fno-rtti -Wall -Wextra)

# Cデモ: C ABI越しに使用（C++実体を含むため libstdc++ を自動リンク）
add_executable(rim_demo examples/rim_demo.c)
target_link_libraries(rim_demo PRIVATE rim)
set_target_properties(rim_demo PROPERTIES LINKER_LANGUAGE CXX)
