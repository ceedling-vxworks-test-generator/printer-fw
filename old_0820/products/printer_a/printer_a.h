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

RIStatus
PrinterA_GetCapability(
    RICapabilityId capabilityId,
    void* capability);
    
// int RIM_TestInjectJobActive(
//     int active);

// int RIM_TestInjectJobId(
//     int jobId);

// int RIM_TestInjectTemperature(
//     double temperature);

// int RIM_TestInjectUpperDoorOpen(
//     int opened);

// int RIM_TestInjectRightDoorOpen(
//     int opened);   

// int RIM_TestInjectLeftDoorOpen(
//     int opened);

// int RIM_TestInjectStapleLevel(
//     int32_t level);

RIStatus
PrinterA_SetErrorList(
    const RI_FAULT_INFO_LIST* list);

RIStatus
PrinterA_GetErrorList(
    RI_FAULT_INFO_LIST* list);
    
#ifdef __cplusplus
}
#endif