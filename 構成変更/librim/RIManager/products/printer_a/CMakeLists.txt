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

        ${CMAKE_CURRENT_SOURCE_DIR}

        ${PROJECT_SOURCE_DIR}/include

        ${PROJECT_SOURCE_DIR}/core/common/include
        ${PROJECT_SOURCE_DIR}/core/adapter/include
        ${PROJECT_SOURCE_DIR}/core/store/include
        ${PROJECT_SOURCE_DIR}/core/storage/include
        ${PROJECT_SOURCE_DIR}/core/capability/include
        ${PROJECT_SOURCE_DIR}/core/publisher/include
)

target_link_libraries(
    rim-product-printer-a
    PUBLIC
        rim-core)