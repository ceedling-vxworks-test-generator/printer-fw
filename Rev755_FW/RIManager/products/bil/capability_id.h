#pragma once

#include "RIId.hpp"

// 製品要求定義書(製品要求定義/ProductRequirement_BIL追記.xlsx, Capabilityシート)の
// CapabilityIdに対応する。

enum
{
    RI_CAPABILITY_PRODUCT_STATE = 0,    // CapabilityId=1
    RI_CAPABILITY_SUPPLY_STATUS,        // CapabilityId=2
    RI_CAPABILITY_PRINT_JOB_STATUS      // CapabilityId=3
};
