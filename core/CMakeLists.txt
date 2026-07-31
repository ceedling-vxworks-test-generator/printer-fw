#
# OBJECT ライブラリにする理由:
#
# rim-core と rim-product-printer-a(下記)は互いのシンボルを参照し合う
# (ProductBinding 経由: rim-core の rim_api.cpp が products 側の実装を呼び、
#  products 側は core の型を使う)。両方を STATIC アーカイブにすると、
# リンカがアーカイブを1パスしか処理しないため、素直な順序ではどちらを
# 先に置いても他方の未解決シンボルが残る(実際に発生することを確認済み)。
#
# OBJECT ライブラリはアーカイブ化されず、最終的に1つの実体(ルートの
# rimanager)へオブジェクトファイルとして直接まとめられるため、
# アーカイブ間の解決順という問題自体が起きない。
#
add_library(
    rim-core
    OBJECT)

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

#
# BUILD_INTERFACE: このリポジトリ内でビルドするとき(add_subdirectory)に使う実パス。
# INSTALL_INTERFACE: install 後に find_package(rimanager) した消費側が使うパス。
# install 時は全ヘッダを1つの include/ 直下へマージする(下記 install(DIRECTORY) 参照)
# ので、消費側からは単一の -I で全レイヤの #include "Xxx.hpp" が解決できる。
#
target_include_directories(
    rim-core
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/common/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/adapter/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/store/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/storage/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/capability/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/core/publisher/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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
