add_library(
    rim-core
    STATIC)

target_include_directories(
    rim-core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include

        ${PROJECT_SOURCE_DIR}/core/common/include
        ${PROJECT_SOURCE_DIR}/core/adapter/include
        ${PROJECT_SOURCE_DIR}/core/store/include
        ${PROJECT_SOURCE_DIR}/core/storage/include
        ${PROJECT_SOURCE_DIR}/core/storage/accessor/include
        ${PROJECT_SOURCE_DIR}/core/capability/include
        ${PROJECT_SOURCE_DIR}/core/publisher/include

        ${PROJECT_SOURCE_DIR}/products/common
        ${PROJECT_SOURCE_DIR}/products/printer_a
)

file(GLOB_RECURSE CORE_SOURCES
    CONFIGURE_DEPENDS

    ${PROJECT_SOURCE_DIR}/core/common/src/*.cpp
    ${PROJECT_SOURCE_DIR}/core/adapter/src/*.cpp
    ${PROJECT_SOURCE_DIR}/core/store/src/*.cpp
    ${PROJECT_SOURCE_DIR}/core/storage/src/*.cpp
    ${PROJECT_SOURCE_DIR}/core/storage/accessor/src/*.cpp
    ${PROJECT_SOURCE_DIR}/core/capability/src/*.cpp
    ${PROJECT_SOURCE_DIR}/core/publisher/src/*.cpp
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
        ${PROJECT_SOURCE_DIR}/include

        ${PROJECT_SOURCE_DIR}/core/common/include
        ${PROJECT_SOURCE_DIR}/core/adapter/include
        ${PROJECT_SOURCE_DIR}/core/store/include
        ${PROJECT_SOURCE_DIR}/core/storage/include
        ${PROJECT_SOURCE_DIR}/core/storage/accessor/include
        ${PROJECT_SOURCE_DIR}/core/capability/include
        ${PROJECT_SOURCE_DIR}/core/publisher/include

        ${PROJECT_SOURCE_DIR}/products/common
        ${PROJECT_SOURCE_DIR}/products/printer_a
)

target_sources(
    rim-core-obj
    PRIVATE
        ${CORE_SOURCES})
