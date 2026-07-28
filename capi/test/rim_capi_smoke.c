/*
 * rim_capi_smoke.c - rim_capi.h が本当にC言語から呼べることを検証するスモークテスト。
 * gtestは使わない(C++専用のため)。ビルドはCコンパイラで行う(CMakeLists.txt参照)。
 */
#include "rim_capi.h"

#include <stdio.h>
#include <assert.h>

static int g_cb_count = 0;

static void on_capability(const rim_machine_capability_t* cap, void* ctx)
{
    (void)ctx;
    if (cap->capabilities.has_env) {
        g_cb_count++;
    }
}

int main(void)
{
    rim_result_t r = rim_init();
    assert(r == RIM_OK);

    rim_subscription_t sub = rim_subscribe(on_capability, NULL, 0);
    assert(sub >= 0);

    r = rim_push(RIM_ID_TEMPERATURE, rim_raw_scalar(25.0), NULL);
    assert(r == RIM_OK);
    rim_dispatch();
    assert(g_cb_count > 0);

    r = rim_push(RIM_ID_MAINTENANCE_COUNT, rim_raw_scalar(5.0), NULL);
    assert(r == RIM_OK);

    /* RawValue::Struct 経路(kBytes)も呼べることを確認する。PrinterAのRuleはどれも
     * スカラのみを受け付けるため、意図的な型不一致(RIM_ERR_CONVERT)で経路を検証する。 */
    struct { unsigned char level; } pkt = { 77 };
    r = rim_push(RIM_ID_TEMPERATURE, rim_raw_struct(&pkt, sizeof pkt), NULL);
    assert(r == RIM_ERR_CONVERT);

    rim_context_t ctx;
    ctx.has_fault_state = true;
    ctx.fault_state     = RIM_FAULT_RAISED;
    ctx.has_scale_x1000 = false;
    ctx.has_key         = false;
    r = rim_push(RIM_ID_FAULT_CODE, rim_raw_scalar(0x10), &ctx);
    assert(r == RIM_OK);
    rim_dispatch();

    rim_printer_status_t st = rim_get_status(RIM_DOMAIN_FAULT, true);
    assert(st.has_data);
    assert(st.data.fault_present);
    assert(st.data.fault_active_count == 1u);
    assert(st.has_capability);
    assert(st.capability.has_error);
    assert(st.capability.err_has_error);

    rim_unsubscribe(sub);
    rim_shutdown();

    printf("rim_capi_smoke: OK\n");
    return 0;
}
