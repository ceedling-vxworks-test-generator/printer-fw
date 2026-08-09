#
# adapter-layer-dev - Adapterレイヤ(構成変更/0808/Rim_new実装)だけを
# 抜き出して、単独でin/outテストができる環境。
#
# 元ツリー(構成変更/0808/Rim_new/librim/RIManager)から、
# Adapterレイヤ本体(core/adapter/)と、それが実際にコンパイルへ必要とする
# 最小限の共通データモデル(core/common, core/storage, core/store, include,
# products/common/ProductDefinition.hpp)だけを無改変で持ち込んでいる。
# Capability/Publisher/Storage(RIS)/DataAccessor等、Adapterと無関係な
# コードは一切含めない。
#
# 製品定義データ(DataItemDefinitionの実体)は本物のprinter_a一式を使わず、
# test/TestProductCatalog.hpp にテスト専用の最小カタログを新規に用意した
# (本物はCapability/Route/Pipeline定義まで引き込むため)。
#
cmake_minimum_required(VERSION 3.20)

project(adapter_layer_dev LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_library(adapter_layer STATIC
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

# gtestは使わない(ネットワーク制限下で取得できないため)。
# 標準ライブラリのみに依存する単独実行ファイルとして、実際に
# ビルド・実行して確認する。
add_executable(adapter_inout_test
    test/AdapterInOutTest.cpp
    test/TestProductCatalog.cpp
)

target_include_directories(adapter_inout_test PRIVATE
    test
)

target_link_libraries(adapter_inout_test PRIVATE adapter_layer)

enable_testing()
add_test(NAME adapter_inout_test COMMAND adapter_inout_test)
