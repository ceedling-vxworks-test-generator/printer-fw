cmake_minimum_required(VERSION 3.20)

project(mainfw_mock LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 既定はDebug(-g付き)。指定を忘れると最適化ビルドになりステップ実行できない
# ため、この単独プロジェクトでは明示的にデフォルトを倒しておく。
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_executable(mainfw_mock src/main.c)

target_include_directories(mainfw_mock PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_directories(mainfw_mock PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib)

target_link_libraries(mainfw_mock PRIVATE rim-product-printer-a)

set_target_properties(mainfw_mock PROPERTIES
    BUILD_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)
