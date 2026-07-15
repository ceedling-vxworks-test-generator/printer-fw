/*
 * rim_types.h - ReactiveInfoManager 共通型（C ABI）
 *
 * 方針（実装言語比較_C_vs_Cpp.md §0 / ReactiveInfoManager概要.md §3）:
 *   - 公開IFはすべて extern "C"。純Cとして解釈・実装できる。
 *   - 型は「基本C」表現（タグ付きunion / 連番enum / has_xxxフラグ）。
 *     std::optional / std::variant は公開面には出さない（内部の受理点のみ §4-b で使用）。
 *
 * このヘッダは C コンパイラでも C++ コンパイラでもそのまま include できる。
 */
#ifndef RIM_TYPES_H
#define RIM_TYPES_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 真偽値: 純C(C99未満)環境も想定し独自定義（0/1のみ） */
typedef int rim_bool;
#define RIM_TRUE  1
#define RIM_FALSE 0

/* 共通結果コード（既存 pf_result 体系に相当。bool+out引数方針の土台） */
typedef enum rim_result {
    RIM_OK = 0,
    RIM_ERR_RULE_NOT_FOUND,
    RIM_ERR_CONVERT,
    RIM_ERR_CLASSIFY,
    RIM_ERR_POST,        /* Queue/Buffer 投入失敗 */
    RIM_ERR_FULL,        /* 固定容量コンテナ満杯 */
    RIM_ERR_KIND_MISMATCH,/* post種別とId分類の不一致（DataStore §6.1） */
    RIM_ERR_INVALID_ARG
} rim_result_t;

/* データ種別ID: 連番enum＋固定長配列前提（unordered_map不使用・§4-5） */
typedef enum rim_data_id {
    RIM_ID_TEMPERATURE = 0,
    RIM_ID_HUMIDITY,
    RIM_ID_PRESSURE,
    RIM_ID_JOB_PROGRESS,
    RIM_ID_FAULT_CODE,
    RIM_ID_COUNT          /* 番兵: 配列サイズに使用 */
} rim_data_id_t;

/* 入力性質（GAP-1）: Adapterが判別しpost選択に用いる */
typedef enum rim_input_kind {
    RIM_KIND_FAULT = 0,        /* 異常系: 喪失不可・高優先 */
    RIM_KIND_OPERATION_REPORT, /* 動作報告系: 順序保証・喪失不可 */
    RIM_KIND_CURRENT_VALUE     /* 最新値系: 上書き可 */
} rim_input_kind_t;

/* DataValue: 正規化後の標準値。C表現のタグ付きunion（§2.1 のC側） */
typedef enum rim_value_type {
    RIM_VT_NONE = 0,
    RIM_VT_CELSIUS_X100,  /* 固定小数: 摂氏×100 */
    RIM_VT_PERCENT,       /* 0..100 */
    RIM_VT_JOB_PROGRESS,  /* 0..100 */
    RIM_VT_FAULT_CODE
} rim_value_type_t;

typedef struct rim_data_value {
    rim_value_type_t type;   /* タグ（手動管理・CentralInputPortが最後の砦で検証） */
    union {
        int32_t  celsius_x100;
        uint8_t  percent;
        uint8_t  job_progress;
        uint32_t fault_code;
    } u;
} rim_data_value_t;

/* FaultState: 異常系Contextで用いるkey値（DataStore §7） */
typedef enum rim_fault_state {
    RIM_FS_NONE = 0,
    RIM_FS_RAISED,
    RIM_FS_CLEARED,
    RIM_FS_ALL_CLEARED,
    RIM_FS_UPDATED_HEAL,
    RIM_FS_UPDATED_ACTIVE
} rim_fault_state_t;

/* DataContext: 変換・反映の補助情報。has_xxxフラグ方式（optional不使用・§2.3） */
typedef struct rim_data_context {
    rim_bool          has_fault_state;
    rim_fault_state_t fault_state;
    rim_bool          has_scale;
    int32_t           scale_x1000;   /* スケール係数×1000（例: 補助情報） */
} rim_data_context_t;

/* DataEntryItem: 層間で受け渡す標準データモデル（DataStore §7.1） */
typedef struct rim_data_entry_item {
    rim_data_id_t       id;
    rim_data_value_t    value;
    rim_data_context_t  context;
} rim_data_entry_item_t;

/* RegistryDomain: ビットマスク（FlagSet<RegistryDomain>の実体 uint32_t・§2.6） */
typedef uint32_t rim_domain_set_t;
#define RIM_DOMAIN_NONE        0u
#define RIM_DOMAIN_FAULT       (1u << 0)
#define RIM_DOMAIN_OPERATION   (1u << 1)
#define RIM_DOMAIN_CURRENT     (1u << 2)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RIM_TYPES_H */
