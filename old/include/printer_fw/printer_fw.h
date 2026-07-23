/**
 * @file printer_fw.h
 * @brief printer-fw 単一エントリヘッダ。利用者はこれを include すればよい。
 *
 * 印刷機の組込み向け共通フレームワーク。
 *   Driver → Raw Data(辞書) → Abstract State(評価器) → Monitor(変化検知+FSM) → Observer → App
 * 設計詳細は docs/basic-design.md / docs/detailed-design.md を参照。
 */
#ifndef PRINTER_FW_H
#define PRINTER_FW_H

#include "printer_fw/pf_config.h"
#include "printer_fw/pf_types.h"
#include "printer_fw/pf_result.h"
#include "printer_fw/pf_log.h"
#include "printer_fw/pf_port.h"
#include "printer_fw/pf_data.h"
#include "printer_fw/pf_state.h"
#include "printer_fw/pf_fsm.h"
#include "printer_fw/pf_monitor.h"
#include "printer_fw/pf_observer.h"
#include "printer_fw/pf_model.h"
#include "printer_fw/pf_core.h"

#define PF_VERSION_MAJOR 1
#define PF_VERSION_MINOR 0
#define PF_VERSION_PATCH 0

#endif /* PRINTER_FW_H */
