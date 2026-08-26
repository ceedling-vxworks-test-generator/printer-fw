#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "rim_api.h"
#include "printer_a_types.h"

#include "printer_a_data_id.h"
#include "printer_a_capability_id.h"
#include "printer_a_facade_id.h"

RIStatus
PrinterA_SetErrorList(
    const RI_FAULT_INFO_LIST* list);

RIStatus
PrinterA_GetErrorList(
    RI_FAULT_INFO_LIST* list);
    
#ifdef __cplusplus
}
#endif