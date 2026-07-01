/**
 * @file pf_port.h
 * @brief プラットフォーム抽象。HW/OS 依存をこの IF 1点に隔離する。
 *
 * core はこのインターフェースだけに依存し、特定 RTOS / MCU に縛られない。
 * 各環境は pf_port_t を1つ実装する（port/pf_port_baremetal.c など）。
 */
#ifndef PRINTER_FW_PF_PORT_H
#define PRINTER_FW_PF_PORT_H

#include "printer_fw/pf_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * プラットフォーム依存の関数群。
 * - critical_enter/exit は短時間・ネスト可能であること。
 * - log / assert_fail は NULL 可（その場合 core は無視する）。
 */
typedef struct {
    void         (*critical_enter)(void);  /**< 排他区間の開始（割込み禁止 or mutex lock） */
    void         (*critical_exit)(void);   /**< 排他区間の終了 */
    pf_time_ms_t (*time_now)(void);        /**< 単調増加の ms 時刻 */
    void         (*log)(const char* msg);  /**< 任意。NULL 可 */
    void         (*assert_fail)(const char* expr, const char* file, int line); /**< 任意。NULL 可 */
} pf_port_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_PORT_H */
