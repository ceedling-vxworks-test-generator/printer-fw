#pragma once

//
// PrinterADataItems - PrinterA機種のid→変換ルール対応表。機種依存の定義は
// ここに集約する(新機種を追加する場合は products/<機種名>/ に同様のものを
// 用意する)。
//

#include "DataItemDefinition.hpp"
#include "ProductDefinition.hpp"
#include "PrinterANormalizationFunctions.hpp"

namespace rim
{

inline const DataItemDefinition kPrinterADataItems[] = {
    {
        RI_DATA_TEMPERATURE_SENSOR_A,
        "TemperatureSensorA",
        "Environment",
        "Value",
        ValueType::kDouble,
        ValueType::kDouble,
        ValueType::kDouble,
        ValueType::kDouble,
        &NormalizeTemperatureSensorA,
        nullptr,
        nullptr,
    },
    {
        RI_DATA_STAPLE_LEVEL,
        "StapleLevel",
        "Consumable",
        "Value",
        ValueType::kDouble,
        ValueType::kInt32,
        ValueType::kInt32,
        ValueType::kInt32,
        &NormalizeStapleLevel,
        nullptr,
        nullptr,
    },
};

inline const ProductDefinition kPrinterAProduct = {
    nullptr, 0,                                                              // domains
    nullptr, 0,                                                              // routes
    kPrinterADataItems, sizeof(kPrinterADataItems) / sizeof(kPrinterADataItems[0]), // dataItems
    nullptr, 0,                                                              // capabilities
    nullptr, 0,                                                              // pipelines
};

} // namespace rim
