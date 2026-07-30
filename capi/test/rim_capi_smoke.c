/*
 * rim_capi_smoke.c - rim_capi.h が本当にC言語から呼べることを検証するスモークテスト。
 * gtestは使わない(C++専用のため)。ビルドはCコンパイラで行う(CMakeLists.txt参照)。
 *
 * ライフサイクル(init/shutdown)とテスト実行は rim_capi_test_main.c が担う。
 */
#include "rim_capi_test.h"
#include "rim_capi.h"

static int g_cb_count = 0;

static void OnCapability(const rim_machine_capability_t* cap, void* ctx)
{
    (void)ctx;
    if (cap->capabilities.has_env) {
        ++g_cb_count;
    }
}

/* 購読 → push → dispatch → コールバック到達 の一連が通ること。 */
RIM_TEST(SmokeSubscribeAndDispatch)
{
    const rim_subscription_t sub = rim_subscribe(OnCapability, NULL, 0);
    RIM_EXPECT(sub >= 0);

    g_cb_count = 0;
    RIM_EXPECT_EQ_I(rim_push(RIM_ID_TEMPERATURE, rim_raw_scalar(298.15), NULL), RIM_OK);
    rim_dispatch();
    RIM_EXPECT(g_cb_count > 0);

    rim_unsubscribe(sub);
}

/* RawValue の3経路(スカラ/構造体/配列)がCから組み立てて渡せること。
 * PrinterAのRuleはどれもスカラ専用なので、kBytes経路は変換拒否で到達を確認する。 */
RIM_TEST(SmokeRawValueFactories)
{
    RIM_EXPECT_EQ_I(rim_push(RIM_ID_MAINTENANCE_COUNT, rim_raw_scalar(5.0), NULL), RIM_OK);

    struct { unsigned char level; } pkt = { 77 };
    RIM_EXPECT_EQ_I(rim_push(RIM_ID_TEMPERATURE, rim_raw_struct(&pkt, sizeof pkt), NULL),
                    RIM_ERR_CONVERT);

    unsigned char cells[3] = { 40, 50, 60 };
    RIM_EXPECT_EQ_I(rim_push(RIM_ID_TEMPERATURE, rim_raw_array(cells, sizeof cells[0], 3), NULL),
                    RIM_ERR_CONVERT);
}

/* Context 付きの Fault 投入 → Accessor で Fault と Capability が読めること。 */
RIM_TEST(SmokeFaultWithContextAndAccessor)
{
    rim_context_t ctx;
    ctx.has_unit        = false;
    ctx.has_fault_state = true;
    ctx.fault_state     = RIM_FAULT_RAISED;
    ctx.has_scale_x1000 = false;
    ctx.has_key         = false;

    RIM_EXPECT_EQ_I(rim_push(RIM_ID_FAULT_CODE, rim_raw_scalar(0x10), &ctx), RIM_OK);
    rim_dispatch();

    const rim_printer_status_t st = rim_get_status(RIM_DOMAIN_FAULT, true);
    RIM_EXPECT(st.has_data);
    RIM_EXPECT(st.data.fault_present);
    RIM_EXPECT_EQ_U(st.data.fault_active_count, 1u);
    RIM_EXPECT(st.has_capability);
    RIM_EXPECT(st.capability.has_error);
    RIM_EXPECT(st.capability.err_has_error);

    /* 後続テストへ影響を残さないよう全クリアしておく。 */
    ctx.fault_state = RIM_FAULT_ALL_CLEARED;
    RIM_EXPECT_EQ_I(rim_push(RIM_ID_FAULT_CODE, rim_raw_scalar(0x10), &ctx), RIM_OK);
    rim_dispatch();
}

/* 不正引数・範囲外ハンドルで落ちないこと。 */
RIM_TEST(SmokeInvalidArgumentsAreRejected)
{
    RIM_EXPECT(rim_subscribe(NULL, NULL, 0) < 0);   /* コールバックNULLは拒否 */

    /* 範囲外/未登録ハンドルの解除は無害に無視されること(クラッシュしない)。 */
    rim_unsubscribe(-1);
    rim_unsubscribe(RIM_SUBSCRIBER_MAX);
    rim_unsubscribe(RIM_SUBSCRIBER_MAX + 100);
}

/* 購読の上限(RIM_SUBSCRIBER_MAX)を超えたら失敗し、解除すれば再登録できること。 */
RIM_TEST(SmokeSubscriberCapacityLimit)
{
    rim_subscription_t handles[RIM_SUBSCRIBER_MAX];
    for (int i = 0; i < RIM_SUBSCRIBER_MAX; ++i) {
        handles[i] = rim_subscribe(OnCapability, NULL, 0);
        RIM_EXPECT(handles[i] >= 0);
    }
    /* 満杯: これ以上は登録できない。 */
    RIM_EXPECT(rim_subscribe(OnCapability, NULL, 0) < 0);

    /* 1つ解放すれば再登録できる。 */
    rim_unsubscribe(handles[0]);
    const rim_subscription_t again = rim_subscribe(OnCapability, NULL, 0);
    RIM_EXPECT(again >= 0);

    rim_unsubscribe(again);
    for (int i = 1; i < RIM_SUBSCRIBER_MAX; ++i) {
        rim_unsubscribe(handles[i]);
    }
}

void RimCapiSmokeTests(void)
{
    RIM_RUN(SmokeSubscribeAndDispatch);
    RIM_RUN(SmokeRawValueFactories);
    RIM_RUN(SmokeFaultWithContextAndAccessor);
    RIM_RUN(SmokeInvalidArgumentsAreRejected);
    RIM_RUN(SmokeSubscriberCapacityLimit);
}
