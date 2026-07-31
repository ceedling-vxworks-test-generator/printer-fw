#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rim_types.h"
#include "rim_capability.h"

/* ------------------------------------------------------------------ */
/* データ投入時に添える文脈                                             */
/* ------------------------------------------------------------------ */

/*
 * 生値がどの単位で送られてきたかを示す。
 *
 * 同じ id へ摂氏でも華氏でも投入でき、規則が内部統一表現へ正規化する
 * (単位ごとに id を増やさずに済ませるための仕組み)。
 * 未指定は RIM_UNIT_NORMALIZED、つまり「既に正規化済み。換算しない」の扱いになる
 * (単位不明を推測して勝手に換算することはしない)。
 *
 * C++ 側 rim::SourceUnit と値が一致していること。ずれたらビルドで検出する
 * (rim_api.cpp の static_assert)。
 */
typedef enum rim_source_unit_t
{
    RIM_UNIT_NORMALIZED = 0,   /* 既に正規化済み(換算しない) */
    RIM_UNIT_CELSIUS    = 1,   /* 摂氏 [degC] */
    RIM_UNIT_FAHRENHEIT = 2    /* 華氏 [degF] */
} rim_source_unit_t;

/*
 * 異常報告の状態。
 *
 * fault_state を指定した投入は「現在値」ではなく **異常報告** として扱われ、
 * 値の格納先ではなく異常一覧へ反映される(判別は Datastore 層が行う)。
 * 異常コードは key で渡す。
 *
 * C++ 側 rim::FaultState と値が一致していること(rim_api.cpp の static_assert)。
 */
typedef enum rim_fault_state_t
{
    RIM_FAULT_NONE           = 0,
    RIM_FAULT_RAISED         = 1,  /* 発生。異常一覧へ登録する */
    RIM_FAULT_CLEARED        = 2,  /* 解除。異常一覧から取り除く */
    RIM_FAULT_ALL_CLEARED    = 3,  /* 全解除。key は不要 */
    RIM_FAULT_UPDATED_HEAL   = 4,  /* 回復状態へ更新 */
    RIM_FAULT_UPDATED_ACTIVE = 5   /* 発生状態へ更新 */
} rim_fault_state_t;

/*
 * データに添える補足情報。has_xxx が false のフィールドは未指定として扱う。
 */
typedef struct rim_context_t
{
    bool              has_unit;
    rim_source_unit_t unit;

    bool              has_fault_state;
    rim_fault_state_t fault_state;

    bool              has_scale_x1000;
    int32_t           scale_x1000;   /* スケール係数 x1000 */

    bool              has_key;
    uint32_t          key;           /* 異常コード等のキー */
} rim_context_t;

/*
 * RIManager C API
 *
 * 旧 API は RIManager_GetEnvironment / _SubscribeError / _TestInjectTemperature …
 * と **機種固有の Capability 名・データ名が関数名に焼き付いて**いた。
 * その結果、Capability を1つ増やすたびに Core の公開 API に手が入っていた。
 *
 * 本 API は「どの Capability か(RICapabilityId)」「どのデータか(dataId)」を
 * 引数で受ける形に統一してある。ID の意味を知っているのは Product と利用者だけで、
 * Core は整数として素通しする。Capability やデータが増えても Core は無改修。
 */

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

    /* 受け側のバッファが足りない */
    RI_BUFFER_TOO_SMALL = -5,

    /* 内部エラー */
    RI_INTERNAL_ERROR = -100

} RIStatus;

/* ------------------------------------------------------------------ */
/* ライフサイクル                                                       */
/* ------------------------------------------------------------------ */

int RIManager_Create(
    RIM_HANDLE* handle);

int RIManager_Destroy(
    RIM_HANDLE handle);

int RIManager_Start(
    RIM_HANDLE handle);

int RIManager_Stop(
    RIM_HANDLE handle);

/* ------------------------------------------------------------------ */
/* Capability の取得                                                    */
/* ------------------------------------------------------------------ */

/*
 * 変化通知を1件取り出す(溜まっていなければ RI_NO_DATA)。
 *
 * buffer には capabilityId に対応する構造体を受け取れる大きさを渡すこと。
 * 実際に書き込まれたバイト数は written に返る(不要なら NULL)。
 * バッファが足りない場合は RI_BUFFER_TOO_SMALL を返し、**通知は消費しない**。
 */
int RIManager_GetCapability(
    RIM_HANDLE handle,
    RICapabilityId capabilityId,
    void* buffer,
    size_t bufferSize,
    size_t* written);

/*
 * 現在値を読み出す(変化の有無によらず、今の値をいつでも読める)。
 * まだ一度も生成されていない Capability は RI_NO_DATA。
 */
int RIManager_GetCurrentCapability(
    RIM_HANDLE handle,
    RICapabilityId capabilityId,
    void* buffer,
    size_t bufferSize,
    size_t* written);

/* 通知が溜まっているか(0/1)。handle 不正時は負値を返す。 */
int RIManager_HasPendingCapability(
    RIM_HANDLE handle,
    RICapabilityId capabilityId);

/* ------------------------------------------------------------------ */
/* 購読                                                                */
/* ------------------------------------------------------------------ */

int RIManager_SubscribeCapability(
    RIM_HANDLE handle,
    RICapabilityId capabilityId,
    RICapabilityCallback callback,
    void* userData,
    uint64_t* subscriptionId);

int RIManager_Unsubscribe(
    RIM_HANDLE handle,
    uint64_t subscriptionId);

/* ------------------------------------------------------------------ */
/* 異常                                                                */
/* ------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------ */
/* データ投入                                                          */
/* ------------------------------------------------------------------ */

/*
 * Adapter の受理点。
 *
 * 値は double 1本で受け、id ごとの規則が内部統一表現へ正規化する
 * (単位換算・クランプなど)。ctx に単位を添えると、同じ id へ摂氏でも華氏でも
 * 投入できる。ctx は NULL 可(その場合は既に正規化済みとして扱う)。
 *
 * double で受けても int32 / uint32 の値は正確に往復する(仮数部53ビット)。
 * 精度が落ちるのは 2^53 を超える整数を渡した場合だけである。
 */
int RIManager_Push(
    RIM_HANDLE handle,
    uint16_t dataId,
    double value,
    const rim_context_t* ctx);

/* ------------------------------------------------------------------ */
/* データ投入(試験用の短縮形)                                          */
/* ------------------------------------------------------------------ */

/*
 * 旧 API の _TestInjectTemperature / _TestInjectUpperDoorOpen … を、
 * データ ID を引数で受ける3本(型ごと)に置き換えたもの。
 * dataId の意味を知っているのは Product と利用者だけである。
 */

int RIManager_TestInjectDouble(
    RIM_HANDLE handle,
    uint16_t dataId,
    double value);

int RIManager_TestInjectInt32(
    RIM_HANDLE handle,
    uint16_t dataId,
    int32_t value);

int RIManager_TestInjectBool(
    RIM_HANDLE handle,
    uint16_t dataId,
    int value);

#ifdef __cplusplus
}
#endif
