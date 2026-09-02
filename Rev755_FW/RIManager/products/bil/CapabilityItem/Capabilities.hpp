#pragma once

#include <array>

#include "capability_id.h"
#include "data_id.h"
#include "CapabilityItemDefinition.hpp"
#include "CapabilityBuilders.hpp"
#include "CapabilityComparators.hpp"
#include "../../common/CommonDiffFunctions.hpp"

// 製品要求定義書(Capabilityシート)に定義された全Capability。

namespace rim
{

inline constexpr std::array<RIDataId, 4>
kProductStateItems =
{
    RI_DATA_LAYER_INIT_STATE,
    RI_DATA_USE_CASE_EXECUTION_STATUS_LIST,
    RI_DATA_FAULT_INFO_LIST,
    RI_DATA_FUNCTION_LIMIT_LIST
};

inline constexpr std::array<RIDataId, 1>
kSupplyStatusItems =
{
    RI_DATA_SUB_POUCH_SENSOR_VALUE_LIST
};

inline constexpr std::array<RIDataId, 1>
kPrintJobStatusItems =
{
    RI_DATA_PRINT_JOB_INFO_LIST
};

inline constexpr CapabilityItemDefinition
kProductStateCapability
{
    RI_CAPABILITY_PRODUCT_STATE,
    "ProductState",
    ValueType::kInt32,
    { ValueType::kInt32, { .i32 = 0 }, 0 },
    BuildProductState,
    IdentityDiff,
    kProductStateItems.data(),
    kProductStateItems.size(),
    CompressionPolicy::KeepLatest,
    100
};

inline constexpr CapabilityItemDefinition
kSupplyStatusCapability
{
    RI_CAPABILITY_SUPPLY_STATUS,
    "SupplyStatus",
    ValueType::kInt32,
    { ValueType::kInt32, { .i32 = 0 }, 0 },
    BuildSupplyStatus,
    IdentityDiff,
    kSupplyStatusItems.data(),
    kSupplyStatusItems.size(),
    CompressionPolicy::KeepLatest,
    100
};

inline constexpr CapabilityItemDefinition
kPrintJobStatusCapability
{
    RI_CAPABILITY_PRINT_JOB_STATUS,
    "PrintJobStatus",
    ValueType::kInt32,
    { ValueType::kInt32, { .i32 = 0 }, 0 },
    BuildPrintJobStatus,
    IdentityDiff,
    kPrintJobStatusItems.data(),
    kPrintJobStatusItems.size(),
    CompressionPolicy::KeepLatest,
    100
};

} // namespace rim
