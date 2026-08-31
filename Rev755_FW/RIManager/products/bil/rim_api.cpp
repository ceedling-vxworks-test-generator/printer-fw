#include "rim_api_common.h"

#include <cstring>

#include "rim_api.h"

namespace
{

template<typename T>
RIStatus GetStruct(
    RIDataId dataId,
    T& value)
{
    RI_BINARY binary{};

    const auto result =
        RIM_GetBinary(
            dataId,
            &binary);

    if (result != RI_SUCCESS)
    {
        return result;
    }

    if (binary.size != sizeof(T))
    {
        return RI_INTERNAL_ERROR;
    }

    std::memcpy(
        &value,
        binary.data,
        sizeof(T));

    return RI_SUCCESS;
}

template<typename T>
RIStatus SetStruct(
    RIDataId dataId,
    const T& value)
{
    return static_cast<RIStatus>(
        RIM_SetBinary(
            dataId,
            &value,
            sizeof(T)));
}

} // namespace

extern "C"
{

RIStatus
SetLayerInitState(
    const layer_init_state_t* state)
{
    if (!state)
    {
        return RI_INVALID_PARAMETER;
    }

    return SetStruct(
        RI_DATA_LAYER_INIT_STATE,
        *state);
}

RIStatus
GetLayerInitState(
    layer_init_state_t* state)
{
    if (!state)
    {
        return RI_INVALID_PARAMETER;
    }

    return GetStruct(
        RI_DATA_LAYER_INIT_STATE,
        *state);
}

RIStatus
SetPrintJobInfoList(
    const print_job_info_t* list)
{
    if (!list)
    {
        return RI_INVALID_PARAMETER;
    }

    return SetStruct(
        RI_DATA_PRINT_JOB_INFO_LIST,
        *list);
}

RIStatus
GetPrintJobInfoList(
    print_job_info_t* list)
{
    if (!list)
    {
        return RI_INVALID_PARAMETER;
    }

    return GetStruct(
        RI_DATA_PRINT_JOB_INFO_LIST,
        *list);
}

RIStatus
SetUnitStateList(
    const unit_state_t* state)
{
    if (!state)
    {
        return RI_INVALID_PARAMETER;
    }

    return SetStruct(
        RI_DATA_UNIT_STATE_LIST,
        *state);
}

RIStatus
GetUnitStateList(
    unit_state_t* state)
{
    if (!state)
    {
        return RI_INVALID_PARAMETER;
    }

    return GetStruct(
        RI_DATA_UNIT_STATE_LIST,
        *state);
}

RIStatus
SetUseCaseExecutionStatusList(
    const pipeline_info_t* status)
{
    if (!status)
    {
        return RI_INVALID_PARAMETER;
    }

    return SetStruct(
        RI_DATA_USE_CASE_EXECUTION_STATUS_LIST,
        *status);
}

RIStatus
GetUseCaseExecutionStatusList(
    pipeline_info_t* status)
{
    if (!status)
    {
        return RI_INVALID_PARAMETER;
    }

    return GetStruct(
        RI_DATA_USE_CASE_EXECUTION_STATUS_LIST,
        *status);
}

RIStatus
SetSubPouchSensorValueList(
    const sub_pouch_sensor_value_t* value)
{
    if (!value)
    {
        return RI_INVALID_PARAMETER;
    }

    return SetStruct(
        RI_DATA_SUB_POUCH_SENSOR_VALUE_LIST,
        *value);
}

RIStatus
GetSubPouchSensorValueList(
    sub_pouch_sensor_value_t* value)
{
    if (!value)
    {
        return RI_INVALID_PARAMETER;
    }

    return GetStruct(
        RI_DATA_SUB_POUCH_SENSOR_VALUE_LIST,
        *value);
}

RIStatus
SetFaultInfoList(
    const FaultInfoList_t* list)
{
    if (!list)
    {
        return RI_INVALID_PARAMETER;
    }

    return SetStruct(
        RI_DATA_FAULT_INFO_LIST,
        *list);
}

RIStatus
GetFaultInfoList(
    FaultInfoList_t* list)
{
    if (!list)
    {
        return RI_INVALID_PARAMETER;
    }

    return GetStruct(
        RI_DATA_FAULT_INFO_LIST,
        *list);
}

RIStatus
SetFunctionLimitList(
    const FunctionLimitList_t* list)
{
    if (!list)
    {
        return RI_INVALID_PARAMETER;
    }

    return SetStruct(
        RI_DATA_FUNCTION_LIMIT_LIST,
        *list);
}

RIStatus
GetFunctionLimitList(
    FunctionLimitList_t* list)
{
    if (!list)
    {
        return RI_INVALID_PARAMETER;
    }

    return GetStruct(
        RI_DATA_FUNCTION_LIMIT_LIST,
        *list);
}

} // extern "C"
