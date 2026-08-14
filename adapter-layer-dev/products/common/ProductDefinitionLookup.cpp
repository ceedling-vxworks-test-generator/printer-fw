//
// ProductDefinitionLookup.cpp - ProductDefinition.hppが宣言する関数群のうち、
// Adapterレイヤが実際に使う FindDataItem(product, RIDataId) だけを実装する。
//
// 本物の実装(0808の products/printer_a/ProductDefinition.cpp)はDomain/
// Route/Capability/Pipelineの定義まで参照するため、Adapter単体環境には
// 持ち込まない。ここは「idからDataItemDefinitionを引く」というAdapterに
// 必要な最小限だけを、機種に依存しない汎用ロジックとして置く。
//

#include "ProductDefinition.hpp"

#include "DataItemDefinition.hpp"

namespace rim
{

const DataItemDefinition* FindDataItem(
    const ProductDefinition& product,
    RIDataId id)
{
    for (std::size_t i = 0; i < product.dataItemCount; ++i)
    {
        if (product.dataItems[i].id == id)
        {
            return &product.dataItems[i];
        }
    }

    return nullptr;
}

} // namespace rim
