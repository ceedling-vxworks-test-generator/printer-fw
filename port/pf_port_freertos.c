/**
 * @file pf_port_freertos.c
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
 *   static void crit_enter(void){ xSemaphoreTake(s_mtx, portMAX_DELAY); }
 *   static void crit_exit (void){ xSemaphoreGive(s_mtx); }
 *   static pf_time_ms_t time_now(void){ return (pf_time_ms_t)(xTaskGetTickCount() * portTICK_PERIOD_MS); }
 *   // ISR文脈から data_set する場合は taskENTER_CRITICAL_FROM_ISR を使う版を別途用意する。
 */

static void crit_enter(void) { /* TODO: xSemaphoreTake / taskENTER_CRITICAL */ }
static void crit_exit (void) { /* TODO: xSemaphoreGive / taskEXIT_CRITICAL */ }
static pf_time_ms_t time_now(void) { return 0u; /* TODO: xTaskGetTickCount() 由来の ms */ }

pf_port_t pf_port_freertos(void)
{
    pf_port_t p;
    p.critical_enter = crit_enter;
    p.critical_exit  = crit_exit;
    p.time_now       = time_now;
    p.log            = NULL;  /* TODO: UART/RTT 等 */
    p.assert_fail    = NULL;  /* TODO: configASSERT 連携 */
    return p;
}
