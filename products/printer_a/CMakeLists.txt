#
# OBJECT にする理由は core/CMakeLists.txt の rim-core と同じ
# (rim-core との循環参照をアーカイブ間ではなくオブジェクト単位で解決するため)。
#
add_library(
    rim-product-printer-a
    OBJECT)

file(GLOB_RECURSE PRODUCT_SOURCES
    CONFIGURE_DEPENDS
    *.cpp)

target_sources(
    rim-product-printer-a
    PRIVATE
        ${PRODUCT_SOURCES})

#
# BUILD_INTERFACE / INSTALL_INTERFACE の考え方は core/CMakeLists.txt と同じ。
# RIMDataId.hpp / PrinterARules.hpp のヘッダ注入(core は #include "RIMDataId.hpp"
# と名前だけで書いており、実体はこのパス解決で機種側から供給される)は
# BUILD_INTERFACE 側でのみ効けばよい。install 後は全ヘッダが同じ include/ 直下に
# マージされるため、INSTALL_INTERFACE は core 側の1本(CMAKE_INSTALL_INCLUDEDIR)で足りる。
#
target_include_directories(
    rim-product-printer-a
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DataItem>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Adapter>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/common/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/adapter/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/store/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/storage/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/capability/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/publisher/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(
    rim-product-printer-a
    PUBLIC
        rim-core)