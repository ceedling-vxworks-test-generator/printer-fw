/**
 * @file pf_port_freertos.cpp
 * @brief FreeRTOS 向けサンプル port 実装（スタブ）。
 *
 * 実機では FreeRTOS のミューテックス/クリティカルセクションに置き換える。
 * 本ファイルは FreeRTOS ヘッダ無しでもビルドできるよう、中身はスタブ（no-op）にしてある。
 * 実装時は下のコメントを有効化すること。
 */
#include "pf_port_samples.h"

/* 実装例（FreeRTOS）:
 *   #include "FreeRTOS.h"
 *   #include "task.h"
 *   #include "semphr.h"
 *   static SemaphoreHandle_t s_mtx;            // 起動時に xSemaphoreCreateMutex()
 *   static void crit_enter(){ xSemaphoreTake(s_mtx, portMAX_DELAY); }
 *   static void crit_exit (){ xSemaphoreGive(s_mtx); }
 *   static pf_time_ms_t time_now(){ return static_cast<pf_time_ms_t>(xTaskGetTickCount() * portTICK_PERIOD_MS); }
 *   // ISR文脈から data_set する場合は taskENTER_CRITICAL_FROM_ISR を使う版を別途用意する。
 */

namespace {

/** @brief クリティカルセクション開始（実機ではミューテックス取得等に置換）。本スタブでは何もしない。 */
void crit_enter() { /* TODO: xSemaphoreTake / taskENTER_CRITICAL */ }
/** @brief crit_enter() に対応する終了処理。本スタブでは何もしない。 */
void crit_exit()  { /* TODO: xSemaphoreGive / taskEXIT_CRITICAL */ }
/** @brief 実機では xTaskGetTickCount() 由来のmsを返す想定。本スタブは常に0を返す。 */
pf_time_ms_t time_now() { return 0u; /* TODO: xTaskGetTickCount() 由来の ms */ }

} // namespace

/**
 * @brief FreeRTOSサンプル用の pf_port_t を組み立てて返す（現状は全てスタブ）。
 *
 * log/assert_fail は未実装（nullptr）。実機移植時は本ファイル冒頭のコメントを
 * 参考に crit_enter/crit_exit/time_now を実装し、UART等でlogも接続すること。
 * @return スタブのコールバックを設定した pf_port_t。
 */
pf_port_t pf_port_freertos()
{
    pf_port_t p;
    p.critical_enter = crit_enter;
    p.critical_exit  = crit_exit;
    p.time_now       = time_now;
    p.log            = nullptr;  /* TODO: UART/RTT 等 */
    p.assert_fail    = nullptr;  /* TODO: configASSERT 連携 */
    return p;
}
