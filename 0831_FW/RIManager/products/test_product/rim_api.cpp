
#include "rim_api_common.h"

#include <memory>
#include <unordered_map>
#include <cstring>

#include "CapabilityAccessor.hpp"

#include "ProductDefinition.hpp"

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
SetErrorList(
    const RI_FAULT_INFO_LIST* list)
{
    if (!list)
    {
        return RI_INVALID_PARAMETER;
    }

    return SetStruct(
        RI_DATA_ERROR_LIST,
        *list);
}

RIStatus
GetErrorList(
    RI_FAULT_INFO_LIST* list)
{
    if (!list)
    {
        return RI_INVALID_PARAMETER;
    }

    return GetStruct(
        RI_DATA_ERROR_LIST,
        *list);
}


}