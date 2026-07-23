/**
 * @file pf_config.h
 * @brief printer-fw コンパイル時設定（静的確保の上限）。
 *
 * ターゲットの資源に合わせ、ビルドシステムの -D かプロジェクト先頭で上書きする。
 * すべて静的確保のため、ここがメモリフットプリントを決める。
 */
#ifndef PRINTER_FW_PF_CONFIG_H
#define PRINTER_FW_PF_CONFIG_H

#ifndef PF_DATA_MAX
#define PF_DATA_MAX        64u    /**< データ辞書エントリ上限 */
#endif

#ifndef PF_STATE_MAX
#define PF_STATE_MAX       32u    /**< 抽象状態の上限 */
#endif

#ifndef PF_OBSERVER_MAX
#define PF_OBSERVER_MAX    16u    /**< 購読者(Observer)の上限 */
#endif

#ifndef PF_FSM_MAX
#define PF_FSM_MAX          8u    /**< FSM管理する状態の上限 */
#endif

#ifndef PF_RAW_POOL_BYTES
#define PF_RAW_POOL_BYTES 1024u   /**< BLOB(構造体/配列/可変長)用の固定プール（0で無効） */
#endif

#endif /* PRINTER_FW_PF_CONFIG_H */
