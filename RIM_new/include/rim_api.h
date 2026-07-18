#pragma once
#ifdef __cplusplus
extern "C" {
#endif
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

int RIManager_GetEnvironment(
    RIM_HANDLE handle,
    void* capability);

int RIManager_GetError(
    RIM_HANDLE handle,
    void* capability);

int RIManager_GetPrintReady(
    RIM_HANDLE handle,
    void* capability);

#ifdef __cplusplus
}
#endif