add_library(
    rim-core
    STATIC)

set(RIM_CORE_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/include

    ${PROJECT_SOURCE_DIR}/core/common/include
    ${PROJECT_SOURCE_DIR}/core/adapter/include
    ${PROJECT_SOURCE_DIR}/core/store/include

    ${PROJECT_SOURCE_DIR}/core/storage/include
    ${PROJECT_SOURCE_DIR}/core/storage/accessor/include
    ${PROJECT_SOURCE_DIR}/core/storage/snapshot/include

    ${PROJECT_SOURCE_DIR}/core/capability/include
    ${PROJECT_SOURCE_DIR}/core/publisher/include

    ${PROJECT_SOURCE_DIR}/products/common
    ${PROJECT_SOURCE_DIR}/products/printer_a
)

target_include_directories(
    rim-core
    PUBLIC
        ${RIM_CORE_INCLUDE_DIRS}
)

target_compile_options(
    rim-core
    PRIVATE
        ${RIM_COMPILE_OPTIONS}
)

file(GLOB_RECURSE CORE_SOURCES
    CONFIGURE_DEPENDS

    ${PROJECT_SOURCE_DIR}/core/common/*.cpp
    ${PROJECT_SOURCE_DIR}/core/adapter/*.cpp
    ${PROJECT_SOURCE_DIR}/core/store/*.cpp
    ${PROJECT_SOURCE_DIR}/core/storage/*.cpp
    ${PROJECT_SOURCE_DIR}/core/capability/*.cpp
    ${PROJECT_SOURCE_DIR}/core/publisher/*.cpp
    ${PROJECT_SOURCE_DIR}/products/common/*.cpp
)

target_sources(
    rim-core
    PRIVATE
        ${CORE_SOURCES})

#
# Object Library
# Shared Library作成用
#

add_library(
    rim-core-obj
    OBJECT)

target_include_directories(
    rim-core-obj
    PUBLIC
        ${RIM_CORE_INCLUDE_DIRS}
)

target_compile_options(
    rim-core-obj
    PRIVATE
        ${RIM_COMPILE_OPTIONS}
)

target_sources(
    rim-core-obj
    PRIVATE
        ${CORE_SOURCES})
