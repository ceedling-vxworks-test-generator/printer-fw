/**
 * @file test_printer_fw.c
 * @brief printer-fw コアの最小ユニットテスト（実アサーション付き）。
 *
 * 依存を増やさないため自己完結のCHECKマクロで書いているが、構造は Unity/ceedling に
 * そのまま移植できる（TEST_ASSERT_* に置換）。`engineering/ceedling/` のテスト自動生成と接続可能。
 *
 * 実行: 失敗が無ければ exit 0、失敗があれば exit 1。
 */
#include "printer_fw/printer_fw.h"
#include "model_sample.h"
#include "model_sample_ids.h"
#include "pf_port_samples.h"
#include <stdio.h>

static int g_fail;
#define CHECK(cond) do { \
    if (!(cond)) { printf("  FAIL: %s (line %d)\n", #cond, __LINE__); g_fail++; } \
} while (0)

/* --- pf_data: 型別 get/set・型不一致・未登録ID・予約ID --- */
static void test_data(void)
{
    printf("[test_data]\n");
    static const pf_data_desc_t td[] = {
        { 0, PF_TYPE_U32,  0, 1, 0, "a" },
        { 1, PF_TYPE_BOOL, 0, 1, 0, "b" },
    };
    CHECK(pf_data_init(td, 2) == PF_OK);

    uint32_t u = 0;
    CHECK(pf_data_set_u32(0, 12345u) == PF_OK);
    CHECK(pf_data_get_u32(0, &u) == PF_OK);
    CHECK(u == 12345u);

    bool b = false;
    CHECK(pf_data_set_bool(1, true) == PF_OK);
    CHECK(pf_data_get_bool(1, &b) == PF_OK);
    CHECK(b == true);

    /* 型不一致 / 未登録ID */
    CHECK(pf_data_get_bool(0, &b) == PF_ERR_TYPE_MISMATCH);
    CHECK(pf_data_get_u32(99, &u) == PF_ERR_NOT_FOUND);

    /* 予約ID 0xFFFF は弾く */
    static const pf_data_desc_t bad[] = { { 0xFFFF, PF_TYPE_U32, 0, 1, 0, "x" } };
    CHECK(pf_data_init(bad, 1) == PF_ERR_INVALID_ARG);

    /* 上の失敗initで状態が壊れないよう、正常initし直す */
    CHECK(pf_data_init(td, 2) == PF_OK);
}

/* --- pf_fsm: 許可/不許可・自己ループ --- */
static void test_fsm(void)
{
    printf("[test_fsm]\n");
    static const pf_fsm_transition_t tr[] = { {0,1}, {1,0}, {1,2} };
    static const pf_fsm_desc_t f = { 0, 0, tr, 3, "f" };

    CHECK(pf_fsm_is_allowed(&f, 0, 1) == true);
    CHECK(pf_fsm_is_allowed(&f, 1, 2) == true);
    CHECK(pf_fsm_is_allowed(&f, 0, 0) == true);   /* 自己ループは許可 */
    CHECK(pf_fsm_is_allowed(&f, 0, 2) == false);  /* 未定義遷移 */
    CHECK(pf_fsm_validate(&f, 0, 2) == PF_ERR_INVALID_TRANSITION);
    CHECK(pf_fsm_validate(&f, 0, 1) == PF_OK);
}

/* --- 統合: 変化時のみ通知 / 特定状態の購読 / 不正遷移の維持 --- */
static int g_notes;
static pf_event_t g_last;
static void cb(const pf_event_t* ev, void* ctx) { (void)ctx; g_notes++; g_last = *ev; }

static void test_monitor_observer(void)
{
    printf("[test_monitor_observer]\n");
    pf_core_deinit();
    pf_port_t port = pf_port_baremetal();
    CHECK(pf_core_init(model_sample_get(), &port) == PF_OK);

    pf_subscription_t h;
    CHECK(pf_observer_subscribe(SAMPLE_STATE_TEMP_ALERT, cb, NULL, &h) == PF_OK);

    pf_data_set_u32 (SAMPLE_RAW_TEMP, 25);
    pf_data_set_enum(SAMPLE_RAW_PHASE, SAMPLE_PHASE_READY);
    pf_data_set_bool(SAMPLE_RAW_DOOR_OPEN, false);

    /* 初回: TEMP_ALERT は -1→NORMAL の変化で1通知（購読は TEMP_ALERT のみ） */
    g_notes = 0;
    CHECK(pf_monitor_tick() == PF_OK);
    CHECK(g_notes == 1);
    CHECK(g_last.new_value == SAMPLE_TEMP_NORMAL);

    /* 変化なし → 通知ゼロ */
    g_notes = 0;
    CHECK(pf_monitor_tick() == PF_OK);
    CHECK(g_notes == 0);

    /* 温度異常 → 1通知 */
    pf_data_set_u32(SAMPLE_RAW_TEMP, 65);
    g_notes = 0;
    CHECK(pf_monitor_tick() == PF_OK);
    CHECK(g_notes == 1);
    CHECK(g_last.new_value == SAMPLE_TEMP_ALARM);

    /* 不正遷移: ERROR→PRINTING は弾かれ MAIN は維持される */
    pf_data_set_enum(SAMPLE_RAW_PHASE, SAMPLE_PHASE_ERROR);
    pf_monitor_tick();   /* READY→…→ERROR（許可経路ではないが、READY→ERROR は許可） */
    int32_t main_v = -1;
    pf_state_get(SAMPLE_STATE_MAIN, &main_v);
    CHECK(main_v == SAMPLE_MAIN_ERROR);

    pf_data_set_enum(SAMPLE_RAW_PHASE, SAMPLE_PHASE_PRINTING); /* ERROR→PRINTING は不許可 */
    pf_monitor_tick();
    pf_state_get(SAMPLE_STATE_MAIN, &main_v);
    CHECK(main_v == SAMPLE_MAIN_ERROR);  /* 維持されている */

    /* unsubscribe 後は通知されない */
    CHECK(pf_observer_unsubscribe(h) == PF_OK);
    pf_data_set_u32(SAMPLE_RAW_TEMP, 25);
    g_notes = 0;
    pf_monitor_tick();
    CHECK(g_notes == 0);

    pf_core_deinit();
}

int main(void)
{
    printf("=== printer-fw unit tests ===\n");
    test_data();
    test_fsm();
    test_monitor_observer();

    if (g_fail == 0) {
        printf("ALL PASS\n");
        return 0;
    }
    printf("FAILED: %d check(s)\n", g_fail);
    return 1;
}
