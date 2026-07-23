/**
 * @file pf_priv.hpp
 * @brief コア内部モジュール間の private インターフェース（公開ヘッダには出さない・非公開）。
 *
 * 各レイヤの実体はクラス化され、モジュールごとに単一のインスタンス（シングルトン、
 * 静的記憶域）として各 .cpp 内に保持される。このヘッダは、他モジュールの .cpp から
 * そのインスタンスの一部機能へアクセスするための内部専用の橋渡し関数を宣言する。
 * 利用者はこのヘッダを直接使わないこと（インストール対象にも含めない）。
 */
#ifndef PRINTER_FW_PF_PRIV_HPP
#define PRINTER_FW_PF_PRIV_HPP

#include "printer_fw/pf_types.h"

/* pf_state（state_registry）内部：monitor_engine が状態を走査・確定値を書き戻すための橋渡し */

/** @brief 登録済みの状態数を返す（monitor_engineが全状態を走査する際のループ上限に使う）。 */
size_t        pf_state__count();
/** @brief slot番目（0..count()-1）の状態IDを返す。 */
pf_state_id_t pf_state__id_at(size_t slot);
/** @brief slot番目の状態の初期値を返す（monitor_engine::init が前回値の初期化に使う）。 */
int32_t       pf_state__initial_at(size_t slot);
/** @brief 確定した状態値をレジストリへ書き戻す（monitor_engineが変化確定時に呼ぶ）。 */
void          pf_state__store(pf_state_id_t id, int32_t value);

#endif /* PRINTER_FW_PF_PRIV_HPP */
