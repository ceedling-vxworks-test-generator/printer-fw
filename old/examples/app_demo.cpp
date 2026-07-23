/**
 * @file app_demo.cpp
 * @brief printer-fw 最小デモ：購読 → raw更新 → 状態変化 → 「変化時のみ」通知 を実証する。
 *
 * 期待動作：
 *   - raw を変えて tick すると、変化した抽象状態だけが通知される。
 *   - 変化が無い tick では通知ゼロ。
 *   - 不正な FSM 遷移（ERROR→PRINTING）は弾かれ、状態は維持される。
 *   - 機種によりドライバのネイティブ型が異なっても（short/long）、辞書の正規値は一致する。
 */
#include "printer_fw/printer_fw.h"
#include "model_sample.h"
#include "model_sample_ids.h"
#include "pf_port_samples.h"
#include <cstdio>

namespace {

/* --- 表示用：状態ID・状態値を文字列へ --- */
const char* state_name(pf_state_id_t id)
{
    switch (id) {
        case SAMPLE_STATE_MAIN:         return "MAIN";
        case SAMPLE_STATE_PRINTABLE:    return "PRINTABLE";
        case SAMPLE_STATE_TEMP_ALERT:   return "TEMP_ALERT";
        case SAMPLE_STATE_VOLUME_LEVEL: return "VOLUME_LEVEL";
        default:                       return "?";
    }
}

const char* value_name(pf_state_id_t id, int32_t v)
{
    if (id == SAMPLE_STATE_MAIN) {
        switch (v) {
            case SAMPLE_MAIN_READY:    return "READY";
            case SAMPLE_MAIN_PRINTING: return "PRINTING";
            case SAMPLE_MAIN_ERROR:    return "ERROR";
            case SAMPLE_MAIN_RECOVERY: return "RECOVERY";
        }
    } else if (id == SAMPLE_STATE_PRINTABLE) {
        return (v == SAMPLE_PRINTABLE_OK) ? "印刷可" : "印刷不可";
    } else if (id == SAMPLE_STATE_TEMP_ALERT) {
        switch (v) {
            case SAMPLE_TEMP_NORMAL: return "正常";
            case SAMPLE_TEMP_WARN:   return "警告";
            case SAMPLE_TEMP_ALARM:  return "異常";
        }
    } else if (id == SAMPLE_STATE_VOLUME_LEVEL) {
        switch (v) {
            case SAMPLE_VOLUME_QUIET:  return "静音";
            case SAMPLE_VOLUME_NORMAL: return "通常";
            case SAMPLE_VOLUME_LOUD:   return "大音量";
        }
    }
    return "?";
}

/* --- 購読者：全状態の変化を表示（UI/ログ部門相当） --- */
int g_notify_count = 0;
void on_change(const pf_event_t* ev, void* ctx)
{
    (void)ctx;
    g_notify_count++;
    std::printf("   [通知] %-10s : %s -> %s  (t=%u)\n",
                state_name(ev->state_id),
                value_name(ev->state_id, ev->old_value),
                value_name(ev->state_id, ev->new_value),
                static_cast<unsigned>(ev->timestamp));
}

/* --- 1サイクル実行（変化があれば on_change が呼ばれる） --- */
void step(const char* title)
{
    g_notify_count = 0;
    std::printf("%s\n", title);
    pf_monitor_tick();
    if (g_notify_count == 0) {
        std::printf("   （変化なし → 通知なし）\n");
    }
}

} // namespace

int main()
{
    /* 実行環境に応じて port を選択：Linux は stdout ログの POSIX port、他は bare-metal サンプル。 */
#if defined(__linux__)
    pf_port_t port = pf_port_linux();
#else
    pf_port_t port = pf_port_baremetal();
#endif
    pf_result_t r = pf_core_init(model_sample_get(), &port);
    if (r != PF_OK) {
        std::printf("init失敗: %s\n", pf_result_str(r));
        return 1;
    }

    pf_subscription_t h;
    pf_observer_subscribe(PF_STATE_ANY, on_change, nullptr, &h);

    std::printf("=== printer-fw デモ（変化時のみ通知）===\n\n");

    /* 初期: 温度25 / READY / カバー閉 */
    pf_data_set_u32 (SAMPLE_RAW_TEMP, 25);
    pf_data_set_enum(SAMPLE_RAW_PHASE, SAMPLE_PHASE_READY);
    pf_data_set_bool(SAMPLE_RAW_DOOR_OPEN, false);
    step("① 初期状態を反映（temp=25, READY, 閉）");

    step("② 同じ条件でもう一度 tick");

    pf_data_set_u32(SAMPLE_RAW_TEMP, 50);
    step("③ 温度=50 に上昇（待機中 → 印刷不可・温度警告）");

    pf_data_set_enum(SAMPLE_RAW_PHASE, SAMPLE_PHASE_PRINTING);
    step("④ 印刷開始（READY → PRINTING）");

    pf_data_set_u32(SAMPLE_RAW_TEMP, 65);
    step("⑤ 温度=65（印刷中 → 温度異常）");

    pf_data_set_enum(SAMPLE_RAW_PHASE, SAMPLE_PHASE_ERROR);
    step("⑥ 異常検知（PRINTING → ERROR）");

    pf_data_set_enum(SAMPLE_RAW_PHASE, SAMPLE_PHASE_PRINTING);
    step("⑦ 不正遷移を試行（ERROR → PRINTING は不許可。MAINは維持される）");

    pf_data_set_enum(SAMPLE_RAW_PHASE, SAMPLE_PHASE_RECOVERY);
    step("⑧ 復帰操作（ERROR → RECOVERY）");

    pf_data_set_u32 (SAMPLE_RAW_TEMP, 25);
    pf_data_set_enum(SAMPLE_RAW_PHASE, SAMPLE_PHASE_READY);
    step("⑨ 復帰成功（RECOVERY → READY、温度正常、印刷可へ）");

    /* --- 機種でドライバのネイティブ型が違っても、辞書に入れば同じ扱いになる実演 --- */
    std::printf("\n--- 参考: 音量ドライバの型差異吸収（Aドライバ=short / Bドライバ=long）---\n");

    model_sample_volume_ingest_from_short(static_cast<short>(30));   /* Aドライバ想定 */
    step("⑩-A Aドライバ(short)で volume=30 を取り込み");
    {
        int32_t v = 0;
        pf_data_get_i32(SAMPLE_RAW_VOLUME, &v);
        std::printf("   [検証] 辞書内の正規値(i32) = %d\n", static_cast<int>(v));
    }

    model_sample_volume_ingest_from_long(30L);          /* Bドライバ想定・同じ物理音量30 */
    step("⑩-B Bドライバ(long)で同じ物理音量=30 を取り込み");
    {
        int32_t v = 0;
        pf_data_get_i32(SAMPLE_RAW_VOLUME, &v);
        std::printf("   [検証] 辞書内の正規値(i32) = %d （Aと同一→カテゴリも変化せず無通知）\n", static_cast<int>(v));
    }

    model_sample_volume_ingest_from_short(static_cast<short>(90));
    step("⑩-C Aドライバ(short)で volume=90 → 大音量へ変化");

    std::printf("\n=== デモ終了 ===\n");
    pf_core_deinit();
    return 0;
}
