#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "rim_types.h"
#include "rim_capability.h"

typedef void (*RIEnvironmentCallback)(
    uint64_t subscriptionId,
    const void* capability,
    void* userData);
typedef void (*RIErrorCallback)(
    uint64_t subscriptionId,
    const void* capability,
    void* userData);
typedef void (*RIPrintReadyCallback)(
    uint64_t subscriptionId,
    const void* capability,
    void* userData);
typedef void (*RIConsumableCallback)(
    uint64_t subscriptionId,
    const void* capability,
    void* userData);
typedef void (*RIJobCallback)(
    uint64_t subscriptionId,
    const void* capability,
    void* userData);


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

int RIManager_Create(
    RIM_HANDLE* handle);

int RIManager_Destroy(
    RIM_HANDLE handle);

int RIManager_Start(
    RIM_HANDLE handle);

int RIManager_Stop(
    RIM_HANDLE handle);



int RIManager_SubscribeEnvironment(
    RIM_HANDLE handle,
    RIEnvironmentCallback callback,
    void* userData,
    uint64_t* subscriptionId);

int RIManager_GetEnvironment(
    RIM_HANDLE handle,
    void* capability);

int RIManager_GetCurrentEnvironment(
    RIM_HANDLE handle,
    void* capability);

int RIManager_SubscribeError(
    RIM_HANDLE handle,
    RIErrorCallback callback,
    void* userData,
    uint64_t* subscriptionId);

int RIManager_GetError(
    RIM_HANDLE handle,
    void* capability);

int RIManager_GetCurrentError(
    RIM_HANDLE handle,
    void* capability);

int RIManager_SubscribePrintReady(
    RIM_HANDLE handle,
    RIPrintReadyCallback callback,
    void* userData,
    uint64_t* subscriptionId);

int RIManager_GetPrintReady(
    RIM_HANDLE handle,
    void* capability);

int RIManager_GetCurrentPrintReady(
    RIM_HANDLE handle,
    void* capability);

int RIManager_SubscribeConsumable(
    RIM_HANDLE handle,
    RIConsumableCallback callback,
    void* userData,
    uint64_t* subscriptionId);

int RIManager_GetCurrentConsumable(
    RIM_HANDLE handle,
    void* capability);

int RIManager_SubscribeJob(
    RIM_HANDLE handle,
    RIJobCallback callback,
    void* userData,
    uint64_t* subscriptionId);

int RIManager_GetCurrentJob(
    RIM_HANDLE handle,
    void* capability);

int RIManager_Unsubscribe(
    RIM_HANDLE handle,
    uint64_t subscriptionId);


int RIManager_AddError(
    RIM_HANDLE handle,
    uint32_t errorCode);

int RIManager_RemoveError(
    RIM_HANDLE handle,
    uint32_t errorCode);

int RIManager_SetErrorState(
    RIM_HANDLE handle,
    uint32_t errorCode,
    int state);




int RIManager_TestInjectJobActive(
    RIM_HANDLE handle,
    int active);

int RIManager_TestInjectJobId(
    RIM_HANDLE handle,
    int jobId);

int RIManager_TestInjectTemperature(
    RIM_HANDLE handle,
    double temperature);

int RIManager_TestInjectUpperDoorOpen(
    RIM_HANDLE handle,
    int opened);

 int RIManager_TestInjectRightDoorOpen(
    RIM_HANDLE handle,
    int opened);   

int RIManager_TestInjectLeftDoorOpen(
    RIM_HANDLE handle,
    int opened);

int RIManager_TestInjectStapleLevel(
    RIM_HANDLE handle,
    int32_t level);

int RIManager_TestInjectJobActive(
    RIM_HANDLE handle,
    int active);

int RIManager_TestInjectJobId(
    RIM_HANDLE handle,
    int jobId);



#ifdef __cplusplus
}
#endif