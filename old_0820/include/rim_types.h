#pragma once

#include <stdint.h>

typedef void* RIM_HANDLE;

typedef uint64_t RIM_SUBSCRIPTION_ID;

typedef struct RI_BINARY
{
    const uint8_t* data;
    uint32_t size;
} RI_BINARY;

// typedef struct RI_FAULT_INFO
// {
//     uint32_t code;
//     uint32_t state;
// } RI_FAULT_INFO;

// typedef struct RI_FAULT_INFO_LIST
// {
//     RI_FAULT_INFO entries[20];
//     uint32_t count;
// } RI_FAULT_INFO_LIST;

typedef enum
{
    RI_TARGET_CAPABILITY = 0,
    RI_TARGET_DATA = 1,
    RI_TARGET_FACADE = 2
} RI_TARGET_TYPE;

typedef enum
{
    RI_TRIGGER_ON_CHANGE = 0,
    RI_TRIGGER_PERIODIC = 1
} RI_NOTIFICATION_TRIGGER;

typedef enum
{
    RI_METHOD_MAILBOX = 0,
    RI_METHOD_CALLBACK = 1,
    RI_METHOD_CUSTOM = 2
} RI_DELIVERY_METHOD;

typedef void (*RINotificationCallback)(
    uint64_t subscriptionId,
    const void* notificationMessage);

typedef struct
{
    RI_TARGET_TYPE targetType;
    uint32_t targetId;
    RI_NOTIFICATION_TRIGGER trigger;
    

} rim_notification_message_t;

typedef struct
{
    RI_TARGET_TYPE targetType;

    uint32_t targetId;

    RI_NOTIFICATION_TRIGGER trigger;

    RI_DELIVERY_METHOD method;

    uint32_t intervalMs;

    RINotificationCallback callback;

} RI_SUBSCRIPTION_REQUEST;
