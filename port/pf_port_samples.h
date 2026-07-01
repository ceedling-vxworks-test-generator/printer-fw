/**
 * @file pf_port_samples.h
 * @brief サンプル port 実装の取得関数。各環境はこれを参考に1つ実装する。
 */
#ifndef PRINTER_FW_PF_PORT_SAMPLES_H
#define PRINTER_FW_PF_PORT_SAMPLES_H

#include "printer_fw/pf_port.h"

#ifdef __cplusplus
extern "C" {
#endif

/** bare-metal 向けサンプル port（割込み禁止ベースの critical を想定）。全環境でビルド可。 */
pf_port_t pf_port_baremetal(void);

/** FreeRTOS 向けサンプル port（ミューテックス想定のスタブ）。 */
pf_port_t pf_port_freertos(void);

/** Linux (POSIX) 向け port。stdout へログ出力（Ubuntu/TeraTerm で閲覧可）。Linux でのみ実装される。 */
pf_port_t pf_port_linux(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_PORT_SAMPLES_H */
