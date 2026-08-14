set(PRODUCT_INCLUDE_DIRS

    ${CMAKE_CURRENT_SOURCE_DIR}

    ${PROJECT_SOURCE_DIR}/include

    ${PROJECT_SOURCE_DIR}/products
    ${PROJECT_SOURCE_DIR}/products/common

    ${PROJECT_SOURCE_DIR}/core/common/include
    ${PROJECT_SOURCE_DIR}/core/adapter/include
    ${PROJECT_SOURCE_DIR}/core/store/include
    ${PROJECT_SOURCE_DIR}/core/storage/include

    ${PROJECT_SOURCE_DIR}/core/storage/accessor/include

    ${PROJECT_SOURCE_DIR}/core/capability/include
    ${PROJECT_SOURCE_DIR}/core/publisher/include
)

add_library(
    rim-product-printer-a
    STATIC)

file(GLOB_RECURSE PRODUCT_SOURCES
    CONFIGURE_DEPENDS
    *.cpp)

target_sources(
    rim-product-printer-a
    PRIVATE
        ${PRODUCT_SOURCES})

target_include_directories(
    rim-product-printer-a
    PUBLIC

        ${PRODUCT_INCLUDE_DIRS}
)

target_link_libraries(
    rim-product-printer-a
    PUBLIC
        rim-core
)

#
# Distribution Package
#
# Core + Product
#

add_library(
    rim-product-printer-a-shared
    SHARED

    ${PRODUCT_SOURCES}

    $<TARGET_OBJECTS:rim-core-obj>
)

target_include_directories(
    rim-product-printer-a-shared
    PUBLIC

        ${PRODUCT_INCLUDE_DIRS}
)

set_target_properties(
    rim-product-printer-a-shared
    PROPERTIES
        OUTPUT_NAME
            "rim-product-printer-a")