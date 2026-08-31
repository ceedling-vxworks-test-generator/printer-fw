#pragma once

#include "rim_types.h"

//
// 製品要求定義(製品要求定義/ProductRequirement_BIL追記.xlsx)のDataシートに
// 定義された各データの型定義。
//
// 配列サイズ(PRODUCT_LAYER_NUM等)は要求定義書に具体的な数値が
// 記載されていないため、暫定値をプレースホルダとして置いている。
// 実機仕様確定後に正しい値へ更新すること。
//

#define BIL_PRODUCT_LAYER_NUM       8
#define BIL_MAX_PRINT_JOB_NUM       16
#define BIL_UNIT_ID_NUM             16
#define BIL_USE_CASE_EXECUTION_NUM  8
#define BIL_PRINT_COLOR_NUM         4
#define BIL_FAULT_INFO_MAX          20
#define BIL_FUNCTION_LIMIT_MAX      20

#ifdef __cplusplus
extern "C" {
#endif

//
// DataId=1 layer_init_state[PRODUCT_LAYER_NUM]
// レイヤの初期化状態配列[レイヤ数] UNINIT/INIT
//

typedef enum BIL_LAYER_INIT_STATE
{
    BIL_LAYER_UNINIT = 0,
    BIL_LAYER_INIT   = 1
} BIL_LAYER_INIT_STATE;

typedef struct layer_init_state_t
{
    BIL_LAYER_INIT_STATE state[BIL_PRODUCT_LAYER_NUM];
} layer_init_state_t;

//
// DataId=2 print_job_info_list[MAX_PRINT_JOB_NUM]
// 印刷ジョブ情報構造体
//

typedef enum BIL_PRINT_JOB_STATE
{
    BIL_JOB_STATE_PRINTABLE = 0,
    BIL_JOB_STATE_PRINTING  = 1
} BIL_PRINT_JOB_STATE;

typedef enum BIL_PRINT_RESOLUTION
{
    BIL_RESOLUTION_STANDARD = 0,
    BIL_RESOLUTION_HIGH     = 1
} BIL_PRINT_RESOLUTION;

typedef struct BIL_PRINT_JOB_ENTRY
{
    uint32_t jobId;
    BIL_PRINT_JOB_STATE state;

    // 使用色(色ごとの使用有無)。bool配列かどうか要求定義書上は未確定のため
    // uint8_t(0/1)で代用している。
    uint8_t useColor[BIL_PRINT_COLOR_NUM];

    BIL_PRINT_RESOLUTION resolution;
    uint8_t shinglingCount;

    // サムネイルへのポインタ。RIManager外(製品側)が所有するバッファを指す。
    const void* thumbnail;
} BIL_PRINT_JOB_ENTRY;

typedef struct print_job_info_t
{
    BIL_PRINT_JOB_ENTRY entries[BIL_MAX_PRINT_JOB_NUM];
    uint8_t jobCount;
    uint32_t selectedJobId;
} print_job_info_t;

//
// DataId=3 unit_state_list[UNIT_ID_NUM]
// UNIT情報配列[ユニット数] UNINIT/IDLE/BUSY
//

typedef enum BIL_UNIT_STATE
{
    BIL_UNIT_UNINIT = 0,
    BIL_UNIT_IDLE   = 1,
    BIL_UNIT_BUSY   = 2
} BIL_UNIT_STATE;

typedef struct unit_state_t
{
    BIL_UNIT_STATE state[BIL_UNIT_ID_NUM];
} unit_state_t;

//
// DataId=4 use_case_execution_status_list[USE_CASE_EXECUTION_NUM]
// 要求実行情報配列[同時実行数] IDLE/BUSY
//

typedef enum BIL_PIPELINE_STATE
{
    BIL_PIPELINE_IDLE = 0,
    BIL_PIPELINE_BUSY = 1
} BIL_PIPELINE_STATE;

typedef struct pipeline_info_t
{
    BIL_PIPELINE_STATE state[BIL_USE_CASE_EXECUTION_NUM];
} pipeline_info_t;

//
// DataId=5 sub_pouch_sensor_value_list[PRINT_COLOR_NUM]
// サブパウチセンサ情報[色数] EMPTY/NEAR EMPTY/NOMINAL/FULL
//

typedef enum BIL_SUB_POUCH_SENSOR_VALUE
{
    BIL_SUB_POUCH_EMPTY      = 0,
    BIL_SUB_POUCH_NEAR_EMPTY = 1,
    BIL_SUB_POUCH_NOMINAL    = 2,
    BIL_SUB_POUCH_FULL       = 3
} BIL_SUB_POUCH_SENSOR_VALUE;

typedef struct sub_pouch_sensor_value_t
{
    BIL_SUB_POUCH_SENSOR_VALUE value[BIL_PRINT_COLOR_NUM];
} sub_pouch_sensor_value_t;

//
// DataId=6 fault_info_list
// エラー情報 エラーリスト[](独自型),有効件数(UINT32)
// core/common/include/RIStatus.hpp等にあるRI_FAULT_INFO_LISTと同趣旨だが、
// 本製品独自の要求定義として別名で定義する。
//

typedef struct BIL_FAULT_INFO
{
    uint32_t code;
    uint32_t state;
} BIL_FAULT_INFO;

typedef struct FaultInfoList_t
{
    BIL_FAULT_INFO entries[BIL_FAULT_INFO_MAX];
    uint32_t count;
} FaultInfoList_t;

//
// DataId=7 function_limit_list
// 機能制限情報 制限機能リスト[](独自型),有効件数(UINT32)
//

typedef struct BIL_FUNCTION_LIMIT
{
    uint32_t code;
    uint32_t state;
} BIL_FUNCTION_LIMIT;

typedef struct FunctionLimitList_t
{
    BIL_FUNCTION_LIMIT entries[BIL_FUNCTION_LIMIT_MAX];
    uint32_t count;
} FunctionLimitList_t;

//
// CapabilityId=1 product_state
// UNINIT,IDLE,BUSY,ERROR,RECOVERY
//

typedef enum BIL_PRODUCT_STATE
{
    BIL_PRODUCT_UNINIT   = 0,
    BIL_PRODUCT_IDLE     = 1,
    BIL_PRODUCT_BUSY     = 2,
    BIL_PRODUCT_ERROR    = 3,
    BIL_PRODUCT_RECOVERY = 4
} BIL_PRODUCT_STATE;

//
// CapabilityId=2,3 supply_status / print_job_status
// NORMAL/ABNORMAL
//

typedef enum BIL_STATUS
{
    BIL_STATUS_NORMAL   = 0,
    BIL_STATUS_ABNORMAL = 1
} BIL_STATUS;

#ifdef __cplusplus
}
#endif
