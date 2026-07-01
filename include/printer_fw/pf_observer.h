/**
 * @file pf_observer.h
 * @brief ④ Observer / Event Dispatcher。Observerパターンの最小IF。
 *
 * 購読対象は Abstract State（Raw ではない）。購読者テーブルは静的確保。
 * コールバックは Monitor 文脈で同期呼び出しされる → 短く・非ブロッキングに保つこと。
 */
#ifndef PRINTER_FW_PF_OBSERVER_H
#define PRINTER_FW_PF_OBSERVER_H

#include "printer_fw/pf_types.h"
#include "printer_fw/pf_result.h"
#include "printer_fw/pf_monitor.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 状態変化コールバック。 */
typedef void (*pf_observer_cb)(const pf_event_t* ev, void* ctx);

/** 購読ハンドル（>=0 有効、<0 無効）。 */
typedef int pf_subscription_t;

/** 全状態を購読する番兵ID（予約。有効IDに使ってはならない）。 */
#define PF_STATE_ANY ((pf_state_id_t)0xFFFFu)

/** Dispatcher を初期化する。 */
pf_result_t pf_observer_init(void);

/**
 * @brief 状態変化を購読する。
 * @param id         監視対象の状態ID。PF_STATE_ANY で全状態。
 * @param cb         コールバック
 * @param ctx        コールバックへ渡す文脈（任意）
 * @param out_handle 解除に使うハンドル（出力）
 * @return PF_OK / PF_ERR_INVALID_ARG / PF_ERR_NO_SPACE
 *
 * @note 再入規約: 配信中の新規 subscribe は次 tick から有効。
 */
pf_result_t pf_observer_subscribe(pf_state_id_t id, pf_observer_cb cb,
                                  void* ctx, pf_subscription_t* out_handle);

/**
 * @brief 購読を解除する。自分のコールバック内からの解除も可（配信ループ後に解放）。
 */
pf_result_t pf_observer_unsubscribe(pf_subscription_t handle);

/**
 * @brief イベントを購読者へ配信する（主に Monitor が呼ぶ）。
 * @note 配信は開始時点の登録スロット集合に対して回る。
 */
pf_result_t pf_observer_dispatch(const pf_event_t* ev);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_OBSERVER_H */
