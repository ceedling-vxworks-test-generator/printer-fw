/*
 * main.c - mainFW を模した最小限のCプログラム。
 *
 * printer-fw(librimanager.so)をまだ本物のmainFWへ結合できていない段階で、
 * 「共有ライブラリとして正しくロードできるか」「ステップ実行でデバッグできるか」を
 * 先に確認するためのモック。実際のmainFWが持つべき最小のライフサイクルを模している:
 *
 *   1. RIManager_Create() で初期化
 *   2. RIManager_Start() でワーカスレッドを起動
 *   3. センサ値をPush(単位付き投入も含む)
 *   4. Capabilityの現在値を読み出す
 *   5. コールバック購読で変化通知を受け取る
 *   6. RIManager_Stop() / RIManager_Destroy() で後始末
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "rim_api.h"
#include "DataItem/rim_data_ids.h"
#include "CapabilityItem/rim_capability_ids.h"
#include "CapabilityItem/rim_capability_types.h"

static int g_callback_count = 0;

static void OnEnvironmentChanged(
    uint64_t subscriptionId,
    RICapabilityId capabilityId,
    const void* data,
    size_t size,
    void* userData)
{
    (void)subscriptionId;
    (void)userData;

    ++g_callback_count;

    if (capabilityId == RIM_CAP_ENVIRONMENT &&
        size == sizeof(rim_environment_capability_t))
    {
        rim_environment_capability_t env;
        memcpy(&env, data, sizeof env);

        printf(
            "[mainfw-mock] callback: temperature=%.2fK humidity=%.2f\n",
            env.temperature,
            env.humidity);
    }
}

/* ワーカスレッドが非同期に反映するのを、条件を見ながら少し待つ。 */
static void WaitFor(
    RICapabilityId capabilityId)
{
    for (int i = 0; i < 200; ++i)
    {
        if (RIManager_HasPendingCapability(capabilityId) == 1)
        {
            return;
        }

        usleep(1000);
    }
}

int main(void)
{
    printf("[mainfw-mock] start\n");

    /* 1. 初期化 */
    if (RIManager_Create() != RI_SUCCESS)
    {
        fprintf(stderr, "[mainfw-mock] RIManager_Create failed\n");
        return 1;
    }

    if (RIManager_Start() != RI_SUCCESS)
    {
        fprintf(stderr, "[mainfw-mock] RIManager_Start failed\n");
        RIManager_Destroy();
        return 1;
    }

    /* 変化通知を受け取る */
    uint64_t subscriptionId = 0;

    if (RIManager_SubscribeCapability(
            RIM_CAP_ENVIRONMENT,
            OnEnvironmentChanged,
            NULL,
            &subscriptionId) != RI_SUCCESS)
    {
        fprintf(stderr, "[mainfw-mock] SubscribeCapability failed\n");
    }

    /* 3. センサ値のPush(単位付き: 摂氏25度 -> ケルビンへ正規化される) */
    rim_context_t ctx;
    memset(&ctx, 0, sizeof ctx);
    ctx.has_unit = true;
    ctx.unit     = RIM_UNIT_CELSIUS;

    if (RIManager_Push(RIM_ID_TEMPERATURE_SENSOR_A, 25.0, &ctx) != RI_SUCCESS)
    {
        fprintf(stderr, "[mainfw-mock] Push(temperature) failed\n");
    }

    WaitFor(RIM_CAP_ENVIRONMENT);

    /* 4. 現在値の読み出し */
    rim_environment_capability_t current;
    memset(&current, 0, sizeof current);

    if (RIManager_GetCurrentCapability(
            RIM_CAP_ENVIRONMENT,
            &current,
            sizeof current,
            NULL) == RI_SUCCESS)
    {
        printf(
            "[mainfw-mock] current: temperature=%.2fK (25degC のはず: 298.15K)\n",
            current.temperature);
    }

    /* 5. 通知の消費(コールバックとは別に、ポーリング読み出しも試す) */
    rim_environment_capability_t notified;
    memset(&notified, 0, sizeof notified);

    const int getResult = RIManager_GetCapability(
        RIM_CAP_ENVIRONMENT,
        &notified,
        sizeof notified,
        NULL);

    printf(
        "[mainfw-mock] GetCapability result=%d, callback_count=%d\n",
        getResult,
        g_callback_count);

    /* 6. 後始末 */
    RIManager_Unsubscribe(subscriptionId);
    RIManager_Stop();
    RIManager_Destroy();

    printf("[mainfw-mock] done\n");

    return 0;
}
