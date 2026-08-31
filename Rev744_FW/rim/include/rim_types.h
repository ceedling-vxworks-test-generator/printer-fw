#pragma once

#include <stdint.h>

typedef void* RIM_HANDLE;

typedef uint64_t RIM_SUBSCRIPTION_ID;

// error list
typedef struct RI_BINARY
{
    const uint8_t* data;
    uint32_t size;
} RI_BINARY;

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
