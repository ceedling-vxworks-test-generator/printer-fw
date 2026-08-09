#include "TestProductCatalog.hpp"

#include "RIMValueFactory.hpp"

namespace
{

// 温度: 正規化なし(そのままdoubleとして採用)。
rim::RIMValue NormalizeTemperature(
    const rim::RIMValue& value,
    const void*)
{
    return value;
}

// ステープルレベル: 0〜100へクランプし、int32へ変換。
rim::RIMValue NormalizeStapleLevel(
    const rim::RIMValue& value,
    const void*)
{
    double v = value.value.d;

    if (v < 0.0)   v = 0.0;
    if (v > 100.0) v = 100.0;

    return rim::RIMValueFactory::CreateInt32(
        static_cast<std::int32_t>(v));
}

const rim::DataItemDefinition kCatalog[] = {
    {
        RI_DATA_TEMPERATURE_SENSOR_A,
        "TemperatureSensorA",
        "Environment",
        "Value",
        rim::ValueType::kDouble,
        rim::ValueType::kDouble,
        rim::ValueType::kDouble,
        rim::ValueType::kDouble,
        &NormalizeTemperature,
        nullptr,
        nullptr,
    },
    {
        RI_DATA_STAPLE_LEVEL,
        "StapleLevel",
        "Consumable",
        "Value",
        rim::ValueType::kDouble,
        rim::ValueType::kInt32,
        rim::ValueType::kInt32,
        rim::ValueType::kInt32,
        &NormalizeStapleLevel,
        nullptr,
        nullptr,
    },
};

} // namespace

namespace test
{

const rim::ProductDefinition kTestProduct = {
    nullptr, 0,                                        // domains
    nullptr, 0,                                        // routes
    kCatalog, sizeof(kCatalog) / sizeof(kCatalog[0]),   // dataItems
    nullptr, 0,                                         // capabilities
    nullptr, 0,                                         // pipelines
};

} // namespace test

namespace rim
{

// AdapterDispatcher.cpp が呼ぶ唯一のオーバーロード。
// (本物のFindDataItem実装は products/printer_a/ProductDefinition.cpp にあるが、
//  Capability/Route/Pipeline定義まで引き込むためAdapter単体環境では使わない)
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
