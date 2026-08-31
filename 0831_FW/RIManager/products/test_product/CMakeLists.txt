set(PRODUCT_INCLUDE_DIRS

    ${CMAKE_CURRENT_SOURCE_DIR}

    ${PROJECT_SOURCE_DIR}/products
    ${PROJECT_SOURCE_DIR}/products/common

    ${PROJECT_SOURCE_DIR}/core/common/include
    ${PROJECT_SOURCE_DIR}/core/adapter/include
    ${PROJECT_SOURCE_DIR}/core/store/include
    ${PROJECT_SOURCE_DIR}/core/storage/include

    ${PROJECT_SOURCE_DIR}/core/storage/accessor/include
    ${PROJECT_SOURCE_DIR}/core/storage/snapshot/include

    ${PROJECT_SOURCE_DIR}/core/capability/include
    ${PROJECT_SOURCE_DIR}/core/publisher/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(
    rim-product
    STATIC)

file(GLOB_RECURSE PRODUCT_SOURCES
    CONFIGURE_DEPENDS
    *.cpp)

target_sources(
    rim-product
    PRIVATE
        ${PRODUCT_SOURCES})

target_include_directories(
    rim-product
    PUBLIC

        ${PRODUCT_INCLUDE_DIRS}
)

target_compile_options(
    rim-product
    PRIVATE
        ${RIM_COMPILE_OPTIONS}
)

target_link_libraries(
    rim-product
    PUBLIC
        rim-core
)

#
# Distribution Package
#
# Core + Product
#

add_library(
    rim-product-shared
    SHARED

    ${PRODUCT_SOURCES}

    $<TARGET_OBJECTS:rim-core-obj>
)

target_compile_options(
    rim-product-shared
    PRIVATE
        ${RIM_COMPILE_OPTIONS}
)

target_link_options(
    rim-product-shared
    PRIVATE
        ${RIM_LINK_OPTIONS}
)

target_include_directories(
    rim-product-shared
    PUBLIC

        ${PRODUCT_INCLUDE_DIRS}
)

set_target_properties(
    rim-product-shared
    PROPERTIES
        OUTPUT_NAME
            "rim-product")