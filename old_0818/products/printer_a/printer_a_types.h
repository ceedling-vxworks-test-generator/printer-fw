#pragma once

#include "rim_types.h"

// error list
typedef struct RI_FAULT_INFO
{
    uint32_t code;
    uint32_t state;
} RI_FAULT_INFO;

typedef struct RI_FAULT_INFO_LIST
{
    RI_FAULT_INFO entries[20];
    uint32_t count;
} RI_FAULT_INFO_LIST;

