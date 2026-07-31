#pragma once

/*
 * rim_capi_test_helpers.h - capi テスト共通の下ごしらえ。
 *
 * 現行ツリー(旧 capi)は呼出側が rim_dispatch() を明示的に呼ぶ同期モデルだった。
 * こちらの実装は RIManager_Start() でワーカスレッドを3本(Datastore/Capability/
 * Publisher)起動し、push した値が背後で非同期に伝播する。そのため「push した
 * 値が全段を通り終えるまで少し待つ」処理が必要になる。
 *
 * ISO C の範囲(nanosleep 等の POSIX API を持ち込まない。ベアメタル/RTOS
 * ターゲットへそのまま持ち込める形を保つ)で待つ必要があるが、**固定回数の
 * 空ループでは不十分**なことが実測で分かった: プロセス内で空ループを繰り返すと
 * CPU の周波数スケーリングが変動し、同じ反復回数でも実時間が一定しない
 * (テストの実行順によって、後方のテストだけ再現性よく詰まる事象を実際に
 * 確認した)。そのため、小さな空ループを1チャンクとして、**条件が満たされるまで
 * チャンクを繰り返す**方式にする(固定時間を信用せず、都度条件を確認する)。
 *
 * さらに、「GetCurrentCapability が成功したら完了」という待ち方には別の穴が
 * あった: 同じハンドルへ複数回 push するテスト(境界値を1つずつ確かめる等)では、
 * **前回 push の値が既に Capability として存在する**ため、今回の push がまだ
 * 反映されていなくても即座に成功してしまい、古い値を返す(実際にこの事故が
 * 起きたことを確認した)。そのため「push する前の値」を控え、**値が変わった
 * ことまで確認してから返す** RimPushAndWaitCurrent() を用意した。
 */

#include <string.h>

#include "rim_api.h"

#include "CapabilityItem/rim_capability_ids.h"
#include "CapabilityItem/rim_capability_types.h"
#include "DataItem/rim_data_ids.h"

enum { kRimSettleChunkIterations = 5000000L };
enum { kRimSettleMaxChunks       = 200 };
enum { kRimMaxCapabilityBytes    = 256 };  /* core 側の kCapabilityPayloadMaxBytes と同じ */

static void RimSettleChunk(void)
{
    volatile long i;
    for (i = 0; i < kRimSettleChunkIterations; ++i) {
        /* 何もしない: ワーカスレッドに実行機会を譲るためだけの空費やし */
    }
}

/* 大まかに1回分待つ(条件無しでとりあえず伝播させたい箇所向け)。 */
static void RimSettle(void)
{
    int i;
    for (i = 0; i < 30; ++i) {
        RimSettleChunk();
    }
}

/* capabilityId の通知が溜まるまで、上限まで待つ。溜まったら true。 */
static int RimWaitPending(RIM_HANDLE h, RICapabilityId capabilityId)
{
    int i;
    for (i = 0; i < kRimSettleMaxChunks; ++i) {
        if (RIManager_HasPendingCapability(h, capabilityId) == 1) return 1;
        RimSettleChunk();
    }
    return RIManager_HasPendingCapability(h, capabilityId) == 1;
}

/*
 * push してから、その push が Capability の現在値に反映されるまで待つ。
 *
 * push する前の値を控えておき、GetCurrentCapability の結果がそれと
 * バイト単位で変わるまで(または初めて生成されるまで)待つ。これにより、
 * 同じ Capability へ複数回 push するテストで古い値を早期に読んでしまう事故を防ぐ。
 *
 * 例外: 今回 push した値が偶然「push する前の値」と一致する場合(例: 同じ温度を
 * 摂氏と華氏で連続して push する)、バイト列は変化しないため差分では検知できない。
 * その場合は上限まで待った上で成功として扱う(固定時間待つ旧方式と同じ扱いに
 * フォールバックする)。実際に反映されたかどうかは、この関数の戻り値ではなく
 * 呼出側が読んだ値そのものを期待値と比較することで検証される。
 *
 * ctx は NULL 可。out には最終的に読めた現在値が書かれる(失敗時は不定)。
 * 戻り値: 反映を確認できたら true(値が変わらなかった場合も、上限まで待てば true)。
 */
static int RimPushAndWaitCurrent(RIM_HANDLE h, uint16_t dataId, double value,
                                  const rim_context_t* ctx,
                                  RICapabilityId capabilityId, void* out, size_t size)
{
    unsigned char before[kRimMaxCapabilityBytes];
    int hadBefore;
    int i;

    if (size > sizeof before) return 0;   /* テスト側の呼び方が誤り */

    hadBefore = (RIManager_GetCurrentCapability(h, capabilityId, before, size, NULL) == RI_SUCCESS);

    if (RIManager_Push(h, dataId, value, ctx) != RI_SUCCESS) return 0;

    for (i = 0; i < kRimSettleMaxChunks; ++i) {
        if (RIManager_GetCurrentCapability(h, capabilityId, out, size, NULL) == RI_SUCCESS) {
            if (!hadBefore || memcmp(out, before, size) != 0) return 1;
        }
        RimSettleChunk();
    }

    /* 変化を検知できなかった(偶然同値だった可能性を含む)。最後にもう一度読み、
     * 何かしら読めていれば成功扱いにする(正しく反映されたかは値の比較に委ねる)。 */
    return RIManager_GetCurrentCapability(h, capabilityId, out, size, NULL) == RI_SUCCESS;
}

/* 単位なしの短縮形。 */
static int RimPushAndWaitCurrentNoContext(RIM_HANDLE h, uint16_t dataId, double value,
                                           RICapabilityId capabilityId, void* out, size_t size)
{
    return RimPushAndWaitCurrent(h, dataId, value, NULL, capabilityId, out, size);
}

/* 単位付きの短縮形。 */
static int RimPushAndWaitCurrentWithUnit(RIM_HANDLE h, uint16_t dataId, double value,
                                          rim_source_unit_t unit,
                                          RICapabilityId capabilityId, void* out, size_t size)
{
    rim_context_t ctx;
    ctx.has_unit = true;
    ctx.unit     = unit;
    ctx.has_fault_state = false;
    ctx.has_scale_x1000 = false;
    ctx.has_key         = false;
    return RimPushAndWaitCurrent(h, dataId, value, &ctx, capabilityId, out, size);
}

/* 単位付きで push する(反映は待たない)短縮形。 */
static int RimPushWithUnit(RIM_HANDLE h, uint16_t dataId, double value, rim_source_unit_t unit)
{
    rim_context_t ctx;
    ctx.has_unit = true;
    ctx.unit     = unit;
    ctx.has_fault_state = false;
    ctx.has_scale_x1000 = false;
    ctx.has_key         = false;
    return RIManager_Push(h, dataId, value, &ctx);
}

/* context なし(=正規化済み扱い)で push する(反映は待たない)短縮形。 */
static int RimPush(RIM_HANDLE h, uint16_t dataId, double value)
{
    return RIManager_Push(h, dataId, value, NULL);
}
