/**
 * @file pf_log.c
 * @brief pf_log の実装。登録済み port の log へ「[レベル] メッセージ」を送る。
 *
 * バッファはスタック上（再入可能）。port または log が未登録なら黙って無視する。
 */
#include "printer_fw/pf_log.h"
#include "printer_fw/pf_core.h"   /* pf_core_port() */
#include <stdarg.h>
#include <stdio.h>

void pf_log_write(pf_log_level_t level, const char* fmt, ...)
{
    const pf_port_t* p = pf_core_port();
    if (!p || !p->log || !fmt) return;

    static const char tag[4] = { 'E', 'W', 'I', 'D' };
    int idx = ((int)level >= 0 && (int)level <= 3) ? (int)level : (int)PF_LOG_INFO;

    char buf[256];
    int n = snprintf(buf, sizeof buf, "[%c] ", tag[idx]);
    if (n < 0 || (size_t)n >= sizeof buf) { p->log(fmt); return; }

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf + n, sizeof buf - (size_t)n, fmt, ap);
    va_end(ap);

    p->log(buf);
}
