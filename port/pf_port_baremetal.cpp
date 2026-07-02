/**
 * @file pf_port_baremetal.cpp
 * @brief bare-metal 向けサンプル port 実装。
 *
 * 実機では critical_enter/exit を「割込み禁止/許可」（例: ARM Cortex-M の __disable_irq()/
 * __enable_irq()、ネスト対応に PRIMASK 退避）に置き換える。time_now は SysTick 等の ms カウンタにする。
 * 本サンプルはホストでビルド・デモするため、critical は no-op、time は単調カウンタ、log は stderr。
 */
#include "pf_port_samples.h"
#include <cstdio>

/* 実機例:
 *   static uint32_t s_primask;
 *   static void crit_enter(void){ s_primask = __get_PRIMASK(); __disable_irq(); }
 *   static void crit_exit (void){ if(!s_primask) __enable_irq(); }
 */
namespace {

/** @brief クリティカルセクション開始（実機では割込み禁止）。ホストでは何もしない（単一文脈のため不要）。 */
void crit_enter() { /* TODO: 実機では割込み禁止。ホストでは no-op */ }
/** @brief crit_enter() に対応する終了処理（実機では割込み許可）。ホストでは何もしない。 */
void crit_exit()  { /* TODO: 実機では割込み許可。ホストでは no-op */ }

pf_time_ms_t s_tick = 0;   ///< 呼ばれるたびに1ずつ増える単調カウンタ（実時間ではない代用値）

/** @brief 呼ばれるたびにインクリメントするだけの単調増加カウンタを返す（実機ではSysTick由来のmsに置換）。 */
pf_time_ms_t time_now() { return ++s_tick; }   /* TODO: 実機は SysTick 由来の ms */

/** @brief ログ文字列をそのまま stderr へ改行付きで出力する（NULLは無視）。 */
void host_log(const char* msg) { if (msg) std::fprintf(stderr, "[pf][log] %s\n", msg); }

} // namespace

/**
 * @brief bare-metalサンプル用の pf_port_t を組み立てて返す。
 *
 * 各コールバックにこのファイル内のstatic関数を割り当てるだけ。assert_fail は
 * このサンプルでは未実装（nullptr）とし、core側で無視される。
 * @return 4つのコールバック（critical_enter/exit・time_now・log）を設定した pf_port_t。
 */
pf_port_t pf_port_baremetal()
{
    pf_port_t p;
    p.critical_enter = crit_enter;
    p.critical_exit  = crit_exit;
    p.time_now       = time_now;
    p.log            = host_log;   /* 実機ではUART等。nullptr可 */
    p.assert_fail    = nullptr;     /* TODO: 実機の assert ハンドラ */
    return p;
}
