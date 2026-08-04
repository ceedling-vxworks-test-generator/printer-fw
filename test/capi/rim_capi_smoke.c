/*
 * rim_capi_smoke.c - rim_api.h が本当に C 言語から呼べることを検証するスモークテスト。
 * gtest は使わない(C++専用のため)。ビルドは C コンパイラで行う(CMakeLists.txt 参照)。
 *
 * ライフサイクル(Create/Start/Stop/Destroy)とテスト実行は rim_capi_test_main.c が担う。
 */
#include "rim_capi_test.h"
#include "rim_capi_test_helpers.h"

static int g_cb_count = 0;

static void OnCapability(uint64_t subscriptionId, RICapabilityId capabilityId,
                          const void* data, size_t size, void* userData)
{
    (void)subscriptionId;
    (void)capabilityId;
    (void)data;
    (void)size;
    (void)userData;
    ++g_cb_count;
}

/* 購読 -> push -> ワーカによる伝播 -> コールバック到達 の一連が通ること。 */
RIM_TEST(SmokeSubscribeAndReceive)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    uint64_t sub = 0;
    RIM_EXPECT_EQ_I(
        RIManager_SubscribeCapability(RIM_CAP_ENVIRONMENT, OnCapability, NULL, &sub),
        RI_SUCCESS);
    RIM_EXPECT(sub != 0);

    g_cb_count = 0;
    RIM_EXPECT_EQ_I(RimPush(RIM_ID_TEMPERATURE_SENSOR_A, 298.15), RI_SUCCESS);

    /* コールバックは Notify() の中で HasPendingCapability の元になる Mailbox
     * への積み込みと同時に呼ばれるため、後者を待てば前者も済んでいる。 */
    RIM_EXPECT(RimWaitPending(RIM_CAP_ENVIRONMENT));
    RIM_EXPECT(g_cb_count > 0);

    RIManager_Unsubscribe(sub);
    RIManager_Stop();
    RIManager_Destroy();
}

/* 現在値の読み出し(GetCurrentCapability)と通知の読み出し(GetCapability)の
 * どちらも Push だけで動くこと。 */
RIM_TEST(SmokeCurrentValueAndNotificationBothWork)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    rim_consumable_capability_t current;
    RIM_EXPECT(RimPushAndWaitCurrentNoContext(
        RIM_ID_STAPLE_LEVEL, 42.0, RIM_CAP_CONSUMABLE, &current, sizeof current));
    RIM_EXPECT_EQ_I(current.stapleLevel, 42);

    rim_consumable_capability_t notified;
    RIM_EXPECT(RimWaitPending(RIM_CAP_CONSUMABLE));
    RIM_EXPECT_EQ_I(
        RIManager_GetCapability(RIM_CAP_CONSUMABLE, &notified, sizeof notified, NULL),
        RI_SUCCESS);
    RIM_EXPECT_EQ_I(notified.stapleLevel, 42);

    RIManager_Stop();
    RIManager_Destroy();
}

/* Context 付きの異常投入 -> ErrorRepository / ErrorCapability に反映されること。 */
RIM_TEST(SmokeFaultWithContextAndAccessor)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    RIM_EXPECT_EQ_I(RIManager_AddError(0x10), RI_SUCCESS);
    RIM_EXPECT(RimWaitPending(RIM_CAP_ERROR));

    rim_error_capability_t error;
    RIM_EXPECT_EQ_I(
        RIManager_GetCurrentCapability(RIM_CAP_ERROR, &error, sizeof error, NULL),
        RI_SUCCESS);
    RIM_EXPECT_EQ_U(error.count, 1u);
    RIM_EXPECT_EQ_U(error.errors[0].errorCode, 0x10u);
    RIM_EXPECT(!error.overflow);

    /* 後続テストへ影響を残さないよう解除しておく。 */
    RIM_EXPECT_EQ_I(RIManager_RemoveError(0x10), RI_SUCCESS);
    RimSettle();

    RIManager_Stop();
    RIManager_Destroy();
}

/* 不正引数・未初期化状態で落ちないこと。 */
RIM_TEST(SmokeInvalidArgumentsAreRejected)
{
    /* 未生成(Create 前)の状態で呼ぶと RI_NOT_INITIALIZED になること。 */
    RIM_EXPECT_EQ_I(RIManager_Destroy(), RI_NOT_INITIALIZED);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_NOT_INITIALIZED);
    RIM_EXPECT_EQ_I(RIManager_Stop(), RI_NOT_INITIALIZED);

    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    /* コールバック NULL は拒否される */
    uint64_t sub = 0;
    RIM_EXPECT_EQ_I(
        RIManager_SubscribeCapability(RIM_CAP_ENVIRONMENT, NULL, NULL, &sub),
        RI_INVALID_PARAMETER);

    /* 未登録の購読 ID の解除は無害に無視される(クラッシュしない)。 */
    RIM_EXPECT_EQ_I(RIManager_Unsubscribe(99999), RI_NO_DATA);

    /* バッファに NULL を渡すと拒否される。 */
    RIM_EXPECT_EQ_I(
        RIManager_GetCurrentCapability(RIM_CAP_ENVIRONMENT, NULL, 16, NULL),
        RI_INVALID_PARAMETER);

    RIManager_Stop();
    RIManager_Destroy();
}

/* 通知バッファが小さすぎる場合、書き込まず・消費もしないこと。 */
RIM_TEST(SmokeSmallBufferDoesNotConsumeNotification)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    RIM_EXPECT_EQ_I(RimPush(RIM_ID_TEMPERATURE_SENSOR_A, 300.0), RI_SUCCESS);
    RIM_EXPECT(RimWaitPending(RIM_CAP_ENVIRONMENT));

    RIM_EXPECT_EQ_I(RIManager_HasPendingCapability(RIM_CAP_ENVIRONMENT), 1);

    char tiny[1];
    RIM_EXPECT_EQ_I(
        RIManager_GetCapability(RIM_CAP_ENVIRONMENT, tiny, sizeof tiny, NULL),
        RI_BUFFER_TOO_SMALL);

    /* 消費されていないので、まだ取り出せる。 */
    RIM_EXPECT_EQ_I(RIManager_HasPendingCapability(RIM_CAP_ENVIRONMENT), 1);

    rim_environment_capability_t env;
    RIM_EXPECT_EQ_I(
        RIManager_GetCapability(RIM_CAP_ENVIRONMENT, &env, sizeof env, NULL),
        RI_SUCCESS);

    RIManager_Stop();
    RIManager_Destroy();
}

/* Create/Destroy を繰り返すと、そのたびに新しい状態で作り直されること
 * (シングルトン化後も、テストケース間の状態分離はこの往復で保たれる)。 */
RIM_TEST(SmokeCreateDestroyCycleGivesFreshState)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    rim_consumable_capability_t cap;
    RIM_EXPECT(RimPushAndWaitCurrentNoContext(
        RIM_ID_STAPLE_LEVEL, 10.0, RIM_CAP_CONSUMABLE, &cap, sizeof cap));
    RIM_EXPECT_EQ_I(cap.stapleLevel, 10);

    RIManager_Stop();
    RIManager_Destroy();

    /* 作り直した後は、前回の値を引きずらない(新規生成時は RI_NO_DATA)。 */
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    RIM_EXPECT_EQ_I(
        RIManager_GetCurrentCapability(RIM_CAP_CONSUMABLE, &cap, sizeof cap, NULL),
        RI_NO_DATA);

    RIManager_Stop();
    RIManager_Destroy();
}

void RimCapiSmokeTests(void)
{
    RIM_RUN(SmokeSubscribeAndReceive);
    RIM_RUN(SmokeCurrentValueAndNotificationBothWork);
    RIM_RUN(SmokeFaultWithContextAndAccessor);
    RIM_RUN(SmokeInvalidArgumentsAreRejected);
    RIM_RUN(SmokeSmallBufferDoesNotConsumeNotification);
    RIM_RUN(SmokeCreateDestroyCycleGivesFreshState);
}
