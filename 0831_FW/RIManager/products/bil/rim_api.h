#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "rim_api_common.h"
#include "types.h"

#include "data_id.h"
#include "capability_id.h"

// 各DataItem(いずれもkBinary、実体は独自C構造体)のSet/Get便利関数。
// 内部的にはRIM_SetBinary/RIM_GetBinaryを呼ぶ。

RIStatus
SetLayerInitState(
    const layer_init_state_t* state);

RIStatus
GetLayerInitState(
    layer_init_state_t* state);

RIStatus
SetPrintJobInfoList(
    const print_job_info_t* list);

RIStatus
GetPrintJobInfoList(
    print_job_info_t* list);

RIStatus
SetUnitStateList(
    const unit_state_t* state);

RIStatus
GetUnitStateList(
    unit_state_t* state);

RIStatus
SetUseCaseExecutionStatusList(
    const pipeline_info_t* status);

RIStatus
GetUseCaseExecutionStatusList(
    pipeline_info_t* status);

RIStatus
SetSubPouchSensorValueList(
    const sub_pouch_sensor_value_t* value);

RIStatus
GetSubPouchSensorValueList(
    sub_pouch_sensor_value_t* value);

RIStatus
SetFaultInfoList(
    const FaultInfoList_t* list);

RIStatus
GetFaultInfoList(
    FaultInfoList_t* list);

RIStatus
SetFunctionLimitList(
    const FunctionLimitList_t* list);

RIStatus
GetFunctionLimitList(
    FunctionLimitList_t* list);

#ifdef __cplusplus
}
#endif
