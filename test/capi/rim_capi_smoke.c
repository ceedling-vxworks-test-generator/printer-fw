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
    RIM_HANDLE handle = NULL;
    RIM_EXPECT_EQ_I(RIManager_Create(&handle), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(handle), RI_SUCCESS);

    uint64_t sub = 0;
    RIM_EXPECT_EQ_I(
        RIManager_SubscribeCapability(handle, RIM_CAP_ENVIRONMENT, OnCapability, NULL, &sub),
        RI_SUCCESS);
    RIM_EXPECT(sub != 0);

    g_cb_count = 0;
    RIM_EXPECT_EQ_I(RimPush(handle, RIM_ID_TEMPERATURE_SENSOR_A, 298.15), RI_SUCCESS);

    /* コールバックは Notify() の中で HasPendingCapability の元になる Mailbox
     * への積み込みと同時に呼ばれるため、後者を待てば前者も済んでいる。 */
    RIM_EXPECT(RimWaitPending(handle, RIM_CAP_ENVIRONMENT));
    RIM_EXPECT(g_cb_count > 0);

    RIManager_Unsubscribe(handle, sub);
    RIManager_Stop(handle);
    RIManager_Destroy(handle);
}

/* 現在値の読み出し(GetCurrentCapability)と通知の読み出し(GetCapability)の
 * どちらも Push だけで動くこと。 */
RIM_TEST(SmokeCurrentValueAndNotificationBothWork)
{
    RIM_HANDLE handle = NULL;
    RIM_EXPECT_EQ_I(RIManager_Create(&handle), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(handle), RI_SUCCESS);

    rim_consumable_capability_t current;
    RIM_EXPECT(RimPushAndWaitCurrentNoContext(
        handle, RIM_ID_STAPLE_LEVEL, 42.0, RIM_CAP_CONSUMABLE, &current, sizeof current));
    RIM_EXPECT_EQ_I(current.stapleLevel, 42);

    rim_consumable_capability_t notified;
    RIM_EXPECT(RimWaitPending(handle, RIM_CAP_CONSUMABLE));
    RIM_EXPECT_EQ_I(
        RIManager_GetCapability(handle, RIM_CAP_CONSUMABLE, &notified, sizeof notified, NULL),
        RI_SUCCESS);
    RIM_EXPECT_EQ_I(notified.stapleLevel, 42);

    RIManager_Stop(handle);
    RIManager_Destroy(handle);
}

/* Context 付きの異常投入 -> ErrorRepository / ErrorCapability に反映されること。 */
RIM_TEST(SmokeFaultWithContextAndAccessor)
{
    RIM_HANDLE handle = NULL;
    RIM_EXPECT_EQ_I(RIManager_Create(&handle), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(handle), RI_SUCCESS);

    RIM_EXPECT_EQ_I(RIManager_AddError(handle, 0x10), RI_SUCCESS);
    RIM_EXPECT(RimWaitPending(handle, RIM_CAP_ERROR));

    rim_error_capability_t error;
    RIM_EXPECT_EQ_I(
        RIManager_GetCurrentCapability(handle, RIM_CAP_ERROR, &error, sizeof error, NULL),
        RI_SUCCESS);
    RIM_EXPECT_EQ_U(error.count, 1u);
    RIM_EXPECT_EQ_U(error.errors[0].errorCode, 0x10u);
    RIM_EXPECT(!error.overflow);

    /* 後続テストへ影響を残さないよう解除しておく。 */
    RIM_EXPECT_EQ_I(RIManager_RemoveError(handle, 0x10), RI_SUCCESS);
    RimSettle();

    RIManager_Stop(handle);
    RIManager_Destroy(handle);
}

/* 不正引数で落ちないこと。 */
RIM_TEST(SmokeInvalidArgumentsAreRejected)
{
    RIM_EXPECT_EQ_I(RIManager_Create(NULL), RI_INVALID_PARAMETER);
    RIM_EXPECT_EQ_I(RIManager_Destroy(NULL), RI_INVALID_HANDLE);
    RIM_EXPECT_EQ_I(RIManager_Start(NULL), RI_INVALID_HANDLE);
    RIM_EXPECT_EQ_I(RIManager_Stop(NULL), RI_INVALID_HANDLE);

    RIM_HANDLE handle = NULL;
    RIM_EXPECT_EQ_I(RIManager_Create(&handle), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(handle), RI_SUCCESS);

    /* コールバック NULL は拒否される */
    uint64_t sub = 0;
    RIM_EXPECT_EQ_I(
        RIManager_SubscribeCapability(handle, RIM_CAP_ENVIRONMENT, NULL, NULL, &sub),
        RI_INVALID_PARAMETER);

    /* 未登録の購読 ID の解除は無害に無視される(クラッシュしない)。 */
    RIM_EXPECT_EQ_I(RIManager_Unsubscribe(handle, 99999), RI_NO_DATA);

    /* バッファに NULL を渡すと拒否される。 */
    RIM_EXPECT_EQ_I(
        RIManager_GetCurrentCapability(handle, RIM_CAP_ENVIRONMENT, NULL, 16, NULL),
        RI_INVALID_PARAMETER);

    RIManager_Stop(handle);
    RIManager_Destroy(handle);
}

/* 通知バッファが小さすぎる場合、書き込まず・消費もしないこと。 */
RIM_TEST(SmokeSmallBufferDoesNotConsumeNotification)
{
    RIM_HANDLE handle = NULL;
    RIM_EXPECT_EQ_I(RIManager_Create(&handle), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(handle), RI_SUCCESS);

    RIM_EXPECT_EQ_I(RimPush(handle, RIM_ID_TEMPERATURE_SENSOR_A, 300.0), RI_SUCCESS);
    RIM_EXPECT(RimWaitPending(handle, RIM_CAP_ENVIRONMENT));

    RIM_EXPECT_EQ_I(RIManager_HasPendingCapability(handle, RIM_CAP_ENVIRONMENT), 1);

    char tiny[1];
    RIM_EXPECT_EQ_I(
        RIManager_GetCapability(handle, RIM_CAP_ENVIRONMENT, tiny, sizeof tiny, NULL),
        RI_BUFFER_TOO_SMALL);

    /* 消費されていないので、まだ取り出せる。 */
    RIM_EXPECT_EQ_I(RIManager_HasPendingCapability(handle, RIM_CAP_ENVIRONMENT), 1);

    rim_environment_capability_t env;
    RIM_EXPECT_EQ_I(
        RIManager_GetCapability(handle, RIM_CAP_ENVIRONMENT, &env, sizeof env, NULL),
        RI_SUCCESS);

    RIManager_Stop(handle);
    RIManager_Destroy(handle);
}

/* 複数のハンドルを同時に開いても互いに干渉しないこと
 * (products/printer_a/Pipeline/PrinterAProductBinding.cpp の静的プールの検証)。 */
RIM_TEST(SmokeMultipleHandlesDoNotInterfere)
{
    RIM_HANDLE a = NULL;
    RIM_HANDLE b = NULL;
    RIM_EXPECT_EQ_I(RIManager_Create(&a), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Create(&b), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(a), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(b), RI_SUCCESS);

    rim_consumable_capability_t capA, capB;
    RIM_EXPECT(RimPushAndWaitCurrentNoContext(
        a, RIM_ID_STAPLE_LEVEL, 10.0, RIM_CAP_CONSUMABLE, &capA, sizeof capA));
    RIM_EXPECT(RimPushAndWaitCurrentNoContext(
        b, RIM_ID_STAPLE_LEVEL, 90.0, RIM_CAP_CONSUMABLE, &capB, sizeof capB));

    RIM_EXPECT_EQ_I(capA.stapleLevel, 10);
    RIM_EXPECT_EQ_I(capB.stapleLevel, 90);

    RIManager_Stop(a);
    RIManager_Stop(b);
    RIManager_Destroy(a);
    RIManager_Destroy(b);
}

void RimCapiSmokeTests(void)
{
    RIM_RUN(SmokeSubscribeAndReceive);
    RIM_RUN(SmokeCurrentValueAndNotificationBothWork);
    RIM_RUN(SmokeFaultWithContextAndAccessor);
    RIM_RUN(SmokeInvalidArgumentsAreRejected);
    RIM_RUN(SmokeSmallBufferDoesNotConsumeNotification);
    RIM_RUN(SmokeMultipleHandlesDoNotInterfere);
}
