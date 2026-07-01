/**
 * @file pf_priv.h
 * @brief コア内部のモジュール間 private インターフェース（公開しない）。
 *
 * 公開ヘッダには出さない、レイヤ間の内部連携用。利用者は使わないこと。
 */
#ifndef PRINTER_FW_PF_PRIV_H
#define PRINTER_FW_PF_PRIV_H

#include "printer_fw/pf_types.h"

/* pf_state 内部：Monitor が状態を走査・確定値を書き戻すための内部API */
size_t        pf_state__count(void);
pf_state_id_t pf_state__id_at(size_t slot);
int32_t       pf_state__initial_at(size_t slot);
void          pf_state__store(pf_state_id_t id, int32_t value);

#endif /* PRINTER_FW_PF_PRIV_H */
