#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "rim_types.h"
#include "RIId.hpp"

typedef void *RIM_HANDLE;
typedef enum RIStatus
{
    RI_SUCCESS = 0,

    /* 引数不正 */
    RI_INVALID_PARAMETER = -1,

    /* ハンドル不正 */
    RI_INVALID_HANDLE = -2,

    /* 未初期化 */
    RI_NOT_INITIALIZED = -3,

    /* データなし */
    RI_NO_DATA = -4,

    // /* 購読なし */
    // RI_NOT_SUBSCRIBED = -5,

    // /* サポート外 */
    // RI_NOT_SUPPORTED = -6,

    /* 内部エラー */
    RI_INTERNAL_ERROR = -100

} RIStatus;


int RIM_Create(void);

int RIM_Destroy(void);

int RIM_Start(void);

int RIM_Stop(void);

RIStatus RIM_Subscribe(
    const RI_SUBSCRIPTION_REQUEST* request,
    uint64_t* subscriptionId);
    
RIStatus
RIM_SubscribeCapability(
    RICapabilityId capabilityId,
    RINotificationCallback callback,
    uint64_t* subscriptionId);

RIStatus RIM_GetNotification(
    uint64_t subscriptionId,
    rim_notification_message_t* notificationMessage);

uint32_t RIM_GetMailboxCount(
    uint64_t subscriptionId);

#ifdef __cplusplus

namespace rim
{
class CapabilityAccessor;
}

rim::CapabilityAccessor*
RIM_GetCapabilityAccessor();

#endif

int RIM_Unsubscribe(
    uint64_t subscriptionId);

RIStatus
RIM_GetBool(
    RIDataId dataId,
    int* value);

    RIStatus
RIM_SetBool(
    RIDataId dataId,
    int value);
    
RIStatus
RIM_GetInt32(
    RIDataId dataId,
    int32_t* value);

RIStatus
RIM_SetInt32(
    RIDataId dataId,
    int32_t value);

RIStatus
RIM_GetDouble(
    RIDataId dataId,
    double* value);

RIStatus
RIM_SetDouble(
    RIDataId dataId,
    double value);

RIStatus
RIM_GetBinary(
    RIDataId dataId,
    RI_BINARY* binary);

int RIM_SetBinary(
    RIDataId dataId,
    const void* data,
    size_t size);

#ifdef __cplusplus
}
#endif