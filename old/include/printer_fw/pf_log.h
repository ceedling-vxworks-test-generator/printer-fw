/**
 * @file pf_log.h
 * @brief 軽量ログ機構。登録済み port の log コールバック経由でレベル付きトレースを出す。
 *
 * - 出力先は pf_port_t.log（Linux では stdout、組込みでは UART など＝TeraTerm で閲覧可能）。
 * - PF_LOG_ENABLE=0 でマクロは完全に消える（組込みで stdio を持たない構成向け）。
 * - PF_LOG_LEVEL でコンパイル時にレベルを絞れる（既定 INFO）。
 */
#ifndef PRINTER_FW_PF_LOG_H
#define PRINTER_FW_PF_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PF_LOG_ERROR = 0,
    PF_LOG_WARN  = 1,
    PF_LOG_INFO  = 2,
    PF_LOG_DEBUG = 3
} pf_log_level_t;

/** ログ機構を有効化するか（0で全マクロ消去）。 */
#ifndef PF_LOG_ENABLE
#define PF_LOG_ENABLE 1
#endif

/** 出力する最大レベル（これ以下を出力）。 */
#ifndef PF_LOG_LEVEL
#define PF_LOG_LEVEL PF_LOG_INFO
#endif

/** 内部：フォーマットして port->log へ送る（マクロ経由で呼ぶ）。 */
void pf_log_write(pf_log_level_t level, const char* fmt, ...);

#if PF_LOG_ENABLE
#  define PF_LOG(level, ...) \
      do { if ((level) <= PF_LOG_LEVEL) pf_log_write((level), __VA_ARGS__); } while (0)
#else
#  define PF_LOG(level, ...) do { (void)0; } while (0)
#endif

#define PF_LOGE(...) PF_LOG(PF_LOG_ERROR, __VA_ARGS__)
#define PF_LOGW(...) PF_LOG(PF_LOG_WARN,  __VA_ARGS__)
#define PF_LOGI(...) PF_LOG(PF_LOG_INFO,  __VA_ARGS__)
#define PF_LOGD(...) PF_LOG(PF_LOG_DEBUG, __VA_ARGS__)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_LOG_H */
