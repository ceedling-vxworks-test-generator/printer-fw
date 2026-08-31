add_library(
    rim-product-template
    STATIC)

target_link_libraries(
    rim-product-template
    PUBLIC
        rim-core)

target_compile_options(
    rim-product-template
    PRIVATE
        ${RIM_COMPILE_OPTIONS}
)
