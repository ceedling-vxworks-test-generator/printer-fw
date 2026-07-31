add_library(
    rim-core
    STATIC)

#
# 組込み向けのビルド指定。
#
# 例外と RTTI を無効にする理由:
#   - 例外: 巻き戻しテーブルが実行イメージを膨らませ、送出時の実行時間が
#           見積もれない。RIM は失敗を戻り値(RIMResult / bool)で返す設計なので
#           そもそも例外を投げない。
#   - RTTI: dynamic_cast / typeid を使っていない。型情報の分だけ無駄になる。
#
# ※ この指定は rim-core とこれをリンクする側(PUBLIC)に伝播する。
#   テストは gtest が例外を使うため、テスト側では打ち消している。
#
target_compile_options(
    rim-core
    PUBLIC
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)

target_include_directories(
    rim-core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include

        ${PROJECT_SOURCE_DIR}/core/common/include
        ${PROJECT_SOURCE_DIR}/core/adapter/include
        ${PROJECT_SOURCE_DIR}/core/store/include
        ${PROJECT_SOURCE_DIR}/core/storage/include
        ${PROJECT_SOURCE_DIR}/core/capability/include
        ${PROJECT_SOURCE_DIR}/core/publisher/include
)

file(GLOB_RECURSE CORE_SOURCES
    CONFIGURE_DEPENDS

    ${PROJECT_SOURCE_DIR}/core/common/src/*.cpp
    ${PROJECT_SOURCE_DIR}/core/adapter/src/*.cpp
    ${PROJECT_SOURCE_DIR}/core/store/src/*.cpp
    ${PROJECT_SOURCE_DIR}/core/storage/src/*.cpp
    ${PROJECT_SOURCE_DIR}/core/capability/src/*.cpp
    ${PROJECT_SOURCE_DIR}/core/publisher/src/*.cpp
)

target_sources(
    rim-core
    PRIVATE
        ${CORE_SOURCES})
