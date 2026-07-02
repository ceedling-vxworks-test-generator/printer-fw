/**
 * @file pf_log.cpp
 * @brief pf_log の実装。登録済み port の log へ「[レベル] メッセージ」を送る。
 *
 * バッファはスタック上（再入可能）。port または log が未登録なら黙って無視する。
 */
#include "printer_fw/pf_log.h"
#include "printer_fw/pf_core.h"
#include <cstdarg>
#include <cstdio>

void pf_log_write(pf_log_level_t level, const char* fmt, ...)
{
    const pf_port_t* p = pf_core_port();
    if (!p || !p->log || !fmt) return;

    static const char tag[4] = { 'E', 'W', 'I', 'D' };
    int idx = (static_cast<int>(level) >= 0 && static_cast<int>(level) <= 3)
                  ? static_cast<int>(level)
                  : static_cast<int>(PF_LOG_INFO);

    char buf[256];
    int n = std::snprintf(buf, sizeof buf, "[%c] ", tag[idx]);
    if (n < 0 || static_cast<size_t>(n) >= sizeof buf) { p->log(fmt); return; }

    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf + n, sizeof buf - static_cast<size_t>(n), fmt, ap);
    va_end(ap);

    p->log(buf);
}
