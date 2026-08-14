#
# adapter-layer-dev - Adapterレイヤ(構成変更/0808/Rim_new実装をベースに、
# IN引数へcontextを追加したもの)だけを抜き出して、単独でin/outテストが
# できる環境。
#
# 元ツリー(構成変更/0808/Rim_new/librim/RIManager)から、
# Adapterレイヤ本体(core/adapter/)と、それが実際にコンパイルへ必要とする
# 最小限の共通データモデル(core/common, core/storage, core/store, include,
# products/common/ProductDefinition.hpp)だけを無改変で持ち込んでいる。
# Capability/Publisher/Storage(RIS)/DataAccessor等、Adapterと無関係な
# コードは一切含めない。
#
# 機種依存の変換ルール(DataItemDefinitionの実体・normalize関数)は
# products/printer_a/ に定義する(本物のprinter_a一式は使わない。
# Capability/Route/Pipeline定義まで引き込むため)。
# products/common/ProductDefinitionLookup.cpp はAdapterが実際に使う
# FindDataItem(id引数のみ)だけを実装した、機種非依存の汎用ロジック。
#
# adapter_layer(core/adapter)とproduct_printer_a(products/)は互いの
# シンボルを参照し合う(AdapterDispatcher.cpp -> FindDataItem、
# PrinterADataItems.hpp -> NormalizeXxx)。両方をSTATICアーカイブのまま
# 別ターゲットにすると、リンカがアーカイブを1パスしか処理しないため
# 未解決シンボルが残ることがある(実際に発生することを確認済み)。
# OBJECTライブラリにしておき、最終的に1つの実体(adapter_inout_test)へ
# まとめることで、アーカイブ間の解決順という問題自体を無くしている。
#
cmake_minimum_required(VERSION 3.20)

project(adapter_layer_dev LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Adapterレイヤ本体(機種非依存)。
add_library(adapter_layer OBJECT
    core/adapter/src/AdapterDispatcher.cpp
    core/adapter/src/PrinterAdapter.cpp
)

target_include_directories(adapter_layer PUBLIC
    core/adapter/include
    core/common/include
    core/storage/include
    core/store/include
    include
    products/common
)

# 機種依存の変換ルール(products/printer_a/)+ 汎用のProductDefinition
# ルックアップ(products/common/)。Adapter本体とは別ターゲットにして、
# 「Adapterのメカニズムと機種固有データは別物」という責務分離を保つ。
add_library(product_printer_a OBJECT
    products/common/ProductDefinitionLookup.cpp
    products/printer_a/PrinterANormalizationFunctions.cpp
)

target_include_directories(product_printer_a PUBLIC
    products/printer_a
)

target_link_libraries(product_printer_a PUBLIC adapter_layer)

# gtestは使わない(ネットワーク制限下で取得できないため)。
# 標準ライブラリのみに依存する単独実行ファイルとして、実際に
# ビルド・実行して確認する。
add_executable(adapter_inout_test
    test/AdapterInOutTest.cpp
    $<TARGET_OBJECTS:adapter_layer>
    $<TARGET_OBJECTS:product_printer_a>
)

target_link_libraries(adapter_inout_test PRIVATE product_printer_a)

enable_testing()
add_test(NAME adapter_inout_test COMMAND adapter_inout_test)
