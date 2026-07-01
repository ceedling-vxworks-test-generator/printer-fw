/**
 * @file pf_port_baremetal.c
 * @brief bare-metal 向けサンプル port 実装。
 *
 * 実機では critical_enter/exit を「割込み禁止/許可」（例: ARM Cortex-M の __disable_irq()/
 * __enable_irq()、ネスト対応に PRIMASK 退避）に置き換える。time_now は SysTick 等の ms カウンタにする。
 * 本サンプルはホストでビルド・デモするため、critical は no-op、time は単調カウンタ、log は stderr。
 */
#include "pf_port_samples.h"
#include <stdio.h>

/* 実機例:
 *   static uint32_t s_primask;
 *   static void crit_enter(void){ s_primask = __get_PRIMASK(); __disable_irq(); }
 *   static void crit_exit (void){ if(!s_primask) __enable_irq(); }
 */
static void crit_enter(void) { /* TODO: 実機では割込み禁止。ホストでは no-op */ }
static void crit_exit (void) { /* TODO: 実機では割込み許可。ホストでは no-op */ }

static pf_time_ms_t s_tick;
static pf_time_ms_t time_now(void) { return ++s_tick; }   /* TODO: 実機は SysTick 由来の ms */

static void host_log(const char* msg) { if (msg) fprintf(stderr, "[pf][log] %s\n", msg); }

pf_port_t pf_port_baremetal(void)
{
    pf_port_t p;
    p.critical_enter = crit_enter;
    p.critical_exit  = crit_exit;
    p.time_now       = time_now;
    p.log            = host_log;   /* 実機ではUART等。NULL可 */
    p.assert_fail    = NULL;       /* TODO: 実機の assert ハンドラ */
    return p;
}
