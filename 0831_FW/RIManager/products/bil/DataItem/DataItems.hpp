#pragma once

#include "data_id.h"
#include "DataItemDefinition.hpp"

#include "../types.h"
#include "../Domain/Domains.hpp"
#include "../Route/Route.hpp"

#include "NormalizationFunctions.hpp"
#include "DataComparators.hpp"

#include "../../common/CommonStoreFunctions.hpp"
#include "../../common/CommonDiffFunctions.hpp"

// 製品要求定義書(Dataシート)に定義された全DataItem。
//
// 要求定義書上のValueTypeは Bool/Int32/Double/Binary のみだが、実際の
// 記載内容(layer_init_state配列, print_job_info_t構造体等)はいずれも
// 固定長配列/構造体であり、単純なスカラ値ではない。RIManagerのValueTypeに
// 構造体/配列を直接表現する型が無いため、既存のErrorList(RI_FAULT_INFO_LIST)
// と同じ方式で「ValueType::kBinary + 製品側で定義したC構造体」として扱う。
// Normalization列は全項目「-」のため、IdentityNormalizeで素通しする。

namespace rim
{

inline constexpr DataItemDefinition
kLayerInitState
{
    RI_DATA_LAYER_INIT_STATE,
    "LayerInitState",
    &kMachineDomain,
    ROUTE_DATA,

    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,

    { ValueType::kBinary, { .bytes = nullptr }, 0 },

    IdentityNormalize,
    IdentityStore,
    IdentityDiff,

    // 起動時に一度だけ発生するデータのため圧縮不要。
    CompressionPolicy::None,
    0
};

inline constexpr DataItemDefinition
kPrintJobInfoList
{
    RI_DATA_PRINT_JOB_INFO_LIST,
    "PrintJobInfoList",
    &kPrintJobDomain,
    ROUTE_DATA,

    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,

    { ValueType::kBinary, { .bytes = nullptr }, 0 },

    IdentityNormalize,
    IdentityStore,
    IdentityDiff,

    // ワースト更新頻度 60回/秒くらい -> 通知を間引く。
    CompressionPolicy::KeepLatest,
    100
};

inline constexpr DataItemDefinition
kUnitStateList
{
    RI_DATA_UNIT_STATE_LIST,
    "UnitStateList",
    &kMachineDomain,
    ROUTE_DATA,

    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,

    { ValueType::kBinary, { .bytes = nullptr }, 0 },

    IdentityNormalize,
    IdentityStore,
    IdentityDiff,

    // ワースト更新頻度 10回/秒くらい。
    CompressionPolicy::KeepLatest,
    100
};

inline constexpr DataItemDefinition
kUseCaseExecutionStatusList
{
    RI_DATA_USE_CASE_EXECUTION_STATUS_LIST,
    "UseCaseExecutionStatusList",
    &kMachineDomain,
    ROUTE_DATA,

    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,

    { ValueType::kBinary, { .bytes = nullptr }, 0 },

    IdentityNormalize,
    IdentityStore,
    IdentityDiff,

    // ワースト更新頻度 1回/秒くらい。
    CompressionPolicy::KeepLatest,
    0
};

inline constexpr DataItemDefinition
kSubPouchSensorValueList
{
    RI_DATA_SUB_POUCH_SENSOR_VALUE_LIST,
    "SubPouchSensorValueList",
    &kMachineDomain,
    ROUTE_DATA,

    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,

    { ValueType::kBinary, { .bytes = nullptr }, 0 },

    IdentityNormalize,
    IdentityStore,
    IdentityDiff,

    // ワースト更新頻度 60回/秒くらい -> 通知を間引く。
    CompressionPolicy::KeepLatest,
    100
};

inline constexpr DataItemDefinition
kFaultInfoList
{
    RI_DATA_FAULT_INFO_LIST,
    "FaultInfoList",
    &kMachineDomain,
    ROUTE_DATA,

    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,

    { ValueType::kBinary, { .bytes = nullptr }, 0 },

    IdentityNormalize,
    IdentityStore,
    IdentityDiff,

    // エラー情報のため、既存のErrorListに倣い圧縮しない(発生/クリアを
    // 取りこぼさないため)。
    CompressionPolicy::None,
    0
};

inline constexpr DataItemDefinition
kFunctionLimitList
{
    RI_DATA_FUNCTION_LIMIT_LIST,
    "FunctionLimitList",
    &kMachineDomain,
    ROUTE_DATA,

    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,
    ValueType::kBinary,

    { ValueType::kBinary, { .bytes = nullptr }, 0 },

    IdentityNormalize,
    IdentityStore,
    IdentityDiff,

    // FaultInfoListと同様、圧縮しない。
    CompressionPolicy::None,
    0
};

} // namespace rim
