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

/**
 * @brief レベル付きログを1行フォーマットして port->log() へ渡す。
 *
 * 内部処理:
 *  1. port または port->log が未登録なら何もせず終了（ログ機構が無い構成でも安全）。
 *  2. レベルを示す1文字タグ（E/W/I/D）を "[X] " という接頭辞としてバッファへ書く。
 *  3. 接頭辞がバッファに収まらないほど巨大な場合は、フォーマット処理を諦めて
 *     fmt をそのまま（可変引数は展開せず）port->log() へ渡すフォールバックにする
 *     （バッファ破損より「タグ無しでも出力される」ことを優先）。
 *  4. 通常時は可変引数(...)を vsnprintf で接頭辞の続きに展開し、完成した1行を渡す。
 *
 * @param level 出力するログの深刻度（ERROR/WARN/INFO/DEBUG）
 * @param fmt   printf形式のフォーマット文字列
 */
void pf_log_write(pf_log_level_t level, const char* fmt, ...)
{
    const pf_port_t* p = pf_core_port();
    if (!p || !p->log || !fmt) return;

    static const char tag[4] = { 'E', 'W', 'I', 'D' };
    int idx = (static_cast<int>(level) >= 0 && static_cast<int>(level) <= 3)
                  ? static_cast<int>(level)
                  : static_cast<int>(PF_LOG_INFO);   // 不正なレベル値はINFO扱いにフォールバック

    char buf[256];
    int n = std::snprintf(buf, sizeof buf, "[%c] ", tag[idx]);
    if (n < 0 || static_cast<size_t>(n) >= sizeof buf) { p->log(fmt); return; }  // 異常時はfmtのみ出力

    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf + n, sizeof buf - static_cast<size_t>(n), fmt, ap);  // 接頭辞の続きへ本文を展開
    va_end(ap);

    p->log(buf);
}
