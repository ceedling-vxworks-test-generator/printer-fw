/**
 * @file pf_port_linux.c
 * @brief Linux (POSIX) 向け port 実装。
 *
 * - critical: pthread mutex（タスク/スレッド間の排他）。
 * - time:     CLOCK_MONOTONIC を ms 化。
 * - log:      stdout へ即時 flush 出力（Ubuntu の端末・TeraTerm のシリアル/SSH で閲覧可能）。
 * - assert:   stderr へ出力して abort。
 *
 * 本ファイルは Linux/POSIX 専用（pthread / clock_gettime を使用）。CMake では UNIX のときのみ
 * ビルドされ、Threads::Threads をリンクする。実機（別MCU/RTOS）では pf_port_freertos.c 等を使う。
 */
#include "pf_port_samples.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

static pthread_mutex_t s_mtx = PTHREAD_MUTEX_INITIALIZER;

static void crit_enter(void) { pthread_mutex_lock(&s_mtx); }
static void crit_exit (void) { pthread_mutex_unlock(&s_mtx); }

static pf_time_ms_t time_now(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return 0u;
    return (pf_time_ms_t)((pf_time_ms_t)ts.tv_sec * 1000u +
                          (pf_time_ms_t)(ts.tv_nsec / 1000000L));
}

static void log_out(const char* msg)
{
    if (!msg) return;
    fputs(msg, stdout);
    fputc('\n', stdout);
    fflush(stdout);   /* シリアル/端末に即時反映（TeraTerm でリアルタイム閲覧） */
}

static void assert_fail(const char* expr, const char* file, int line)
{
    fprintf(stderr, "[pf][ASSERT] %s at %s:%d\n", expr ? expr : "?", file ? file : "?", line);
    fflush(stderr);
    abort();
}

pf_port_t pf_port_linux(void)
{
    pf_port_t p;
    p.critical_enter = crit_enter;
    p.critical_exit  = crit_exit;
    p.time_now       = time_now;
    p.log            = log_out;
    p.assert_fail    = assert_fail;
    return p;
}
