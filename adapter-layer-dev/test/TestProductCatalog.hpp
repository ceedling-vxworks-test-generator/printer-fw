#pragma once

//
// TestProductCatalog - Adapterレイヤ単体でのin/outテストのためだけの、
// 最小限のダミー製品定義。
//
// 本物の製品定義(products/printer_a/...)はCapability/Route/Pipeline等
// Adapterレイヤと無関係な定義まで大量に引き込むため、ここでは含めない。
// AdapterDispatcher::Dispatch() が実際に呼ぶ
// FindDataItem(const ProductDefinition&, RIDataId) だけを、
// このテスト専用カタログ向けに実装する(定義本体は TestProductCatalog.cpp)。
//
// AdapterDispatcher / PrinterAdapter 本体のコードは
// 構成変更/0808/Rim_new/librim/RIManager/core/adapter/ から無改変で
// 持ち込んでいる。ここにあるのはテスト用のデータ・配線だけ。
//

#include "DataItemDefinition.hpp"
#include "ProductDefinition.hpp"

namespace test
{

extern const rim::ProductDefinition kTestProduct;

} // namespace test
