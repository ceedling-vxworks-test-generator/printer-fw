#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum RICapabilityId
{
    RI_CAPABILITY_ENVIRONMENT = 0,
    RI_CAPABILITY_PRINT_READY,
    RI_CAPABILITY_CONSUMABLE,
    RI_CAPABILITY_JOB,
    RI_CAPABILITY_ERROR

} RICapabilityId;

#ifdef __cplusplus
}
#endif
