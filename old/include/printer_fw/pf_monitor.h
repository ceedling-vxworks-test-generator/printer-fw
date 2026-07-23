/**
 * @file pf_monitor.h
 * @brief ③ State Monitor Layer ＝ 変化検知（＋FSM・デバウンス）。
 *
 * 全（対象）抽象状態を再評価し、前回値と比較して「変化したものだけ」をイベント化する。
 * 変化が無いサイクルではイベントゼロ＝CPU/帯域を消費しない。
 */
#ifndef PRINTER_FW_PF_MONITOR_H
#define PRINTER_FW_PF_MONITOR_H

#include "printer_fw/pf_types.h"
#include "printer_fw/pf_result.h"
#include "printer_fw/pf_fsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 状態変化イベント。Observer へ配信される。 */
typedef struct {
    pf_state_id_t state_id;
    int32_t       old_value;
    int32_t       new_value;
    pf_time_ms_t  timestamp;
} pf_event_t;

/** Monitor 構成。 */
typedef struct {
    const pf_fsm_desc_t* fsm_list;   /**< FSM管理する状態（任意、NULL/0可） */
    size_t               fsm_count;  /**< FSM数（0でFSM無効＝ゼロコスト） */
    uint8_t              debounce_n; /**< N回連続一致で確定（0/1=即時確定） */
} pf_monitor_cfg_t;

/** Monitor を初期化する。 */
pf_result_t pf_monitor_init(const pf_monitor_cfg_t* cfg);

/** 1サイクル：全状態を再評価 → 変化検知 → 変化分のみ dispatch。 */
pf_result_t pf_monitor_tick(void);

/** 指定状態の現在値を強制的に1回通知する（初期同期等）。 */
pf_result_t pf_monitor_force_emit(pf_state_id_t id);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_MONITOR_H */
