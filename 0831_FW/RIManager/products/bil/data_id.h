#pragma once

#include "RIId.hpp"

// 製品要求定義書(製品要求定義/ProductRequirement_BIL追記.xlsx, Dataシート)の
// DataIdに対応する。

enum
{
    RI_DATA_UNKNOWN = 0,

    RI_DATA_LAYER_INIT_STATE,                  // DataId=1
    RI_DATA_PRINT_JOB_INFO_LIST,                // DataId=2
    RI_DATA_UNIT_STATE_LIST,                    // DataId=3
    RI_DATA_USE_CASE_EXECUTION_STATUS_LIST,     // DataId=4
    RI_DATA_SUB_POUCH_SENSOR_VALUE_LIST,        // DataId=5
    RI_DATA_FAULT_INFO_LIST,                    // DataId=6
    RI_DATA_FUNCTION_LIMIT_LIST                 // DataId=7
};
