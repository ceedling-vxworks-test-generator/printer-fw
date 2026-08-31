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
#include "facade_id.h"

RIStatus
SetErrorList(
    const RI_FAULT_INFO_LIST* list);

RIStatus
GetErrorList(
    RI_FAULT_INFO_LIST* list);
    
#ifdef __cplusplus
}
#endif