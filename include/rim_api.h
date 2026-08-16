#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "rim_types.h"

#include "rim_data_id.h"
#include "rim_capability_id.h"

/*
 * 生値がどの単位で送られてきたかを示す(Adapter の DataItemDefinition::normalize が
 * 解釈する context の一種)。C++ 側 rim::SourceUnit と値が一致していること
 * (rim_api.cpp の static_assert)。
 *
 * RIM_SetDouble 等の ctx 引数へ、このポインタを渡すことで単位換算を指定できる。
 * ctx=NULL は RIM_UNIT_NORMALIZED と同義(既に正規化済み。換算しない)。
 * どの id が実際に unit を解釈するかは DataItemDefinition::normalize 次第であり、
 * 解釈しない id へ渡しても無視されるだけである。
 */
typedef enum rim_source_unit_t
{
    RIM_UNIT_NORMALIZED = 0,
    RIM_UNIT_CELSIUS    = 1,
    RIM_UNIT_FAHRENHEIT = 2
} rim_source_unit_t;

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

RIStatus
RIM_GetCapability(
    RICapabilityId capabilityId,
    void* capability);

int RIM_Unsubscribe(
    uint64_t subscriptionId);




int RIM_TestInjectJobActive(
    int active);

int RIM_TestInjectJobId(
    int jobId);

int RIM_TestInjectTemperature(
    double temperature);

int RIM_TestInjectUpperDoorOpen(
    int opened);

int RIM_TestInjectRightDoorOpen(
    int opened);   

int RIM_TestInjectLeftDoorOpen(
    int opened);

int RIM_TestInjectStapleLevel(
    int32_t level);


RIStatus
RIM_GetBool(
    RIDataId dataId,
    int* value);

/*
 * RIM_SetBool/SetInt32/SetDouble - Adapter(AdapterDispatcher::Dispatch)を経由する
 * 受理点。dataId で DataItemDefinition(=変換規則)を引き、その normalize が
 * value を正規化してから Store へ積む。dataId に対応する DataItemDefinition が
 * 無ければ RI_INVALID_PARAMETER。
 *
 * ctx は normalize へそのまま渡される補足情報で、dataId ごとに解釈が異なる
 * (例: 温度なら rim_source_unit_t へのポインタ。不要なら NULL でよい)。
 */
    RIStatus
RIM_SetBool(
    RIDataId dataId,
    int value,
    const void* ctx);

RIStatus
RIM_GetInt32(
    RIDataId dataId,
    int32_t* value);

RIStatus
RIM_SetInt32(
    RIDataId dataId,
    int32_t value,
    const void* ctx);

RIStatus
RIM_GetDouble(
    RIDataId dataId,
    double* value);

RIStatus
RIM_SetDouble(
    RIDataId dataId,
    double value,
    const void* ctx);

RIStatus
RIM_GetBinary(
    RIDataId dataId,
    RI_BINARY* binary);

int RIM_SetBinary(
    RIDataId dataId,
    const void* data,
    size_t size);

RIStatus
RIM_SetErrorList(
    const RI_FAULT_INFO_LIST* list);

RIStatus
RIM_GetErrorList(
    RI_FAULT_INFO_LIST* list);
    




#ifdef __cplusplus
}
#endif