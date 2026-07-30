#pragma once

/*
 * rim_capi.h - RIM を C言語コードから利用するための唯一の公開ヘッダ。
 *
 * 内部実装はC++(framework/devices)のまま、この層だけが外部(C言語のmainFW)へ露出する
 * extern "C" な薄いシムである。C++固有の機能(クラス/参照/デフォルト引数/テンプレート/
 * std::optional/名前空間)は一切現れない。
 *
 * 使い方の骨子:
 *   rim_init();
 *   rim_push(RIM_ID_TEMPERATURE, rim_raw_scalar(25.0), NULL);
 *   rim_dispatch();
 *   rim_subscribe(my_cb, my_ctx, 0);   // 0 = 全Capabilityに関心
 *   ...
 *   rim_shutdown();
 *
 * id一覧(rim_id_t)は機種固有のため、このヘッダではなく devices/<機種>/inc/capi/rim_ids.h
 * が提供する(datastore/RIMDataId.hpp と同じ「機種側が語彙を持つ」流儀)。
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "capi/rim_ids.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * 結果コード(rim::RIMResult のCミラー。整数値を一致させること)
 * ============================================================ */
typedef enum {
    RIM_OK = 0,
    RIM_ERR_CONVERT,
    RIM_ERR_CLASSIFY,
    RIM_ERR_POST,
    RIM_ERR_KIND_MISMATCH,
    RIM_ERR_INVALID_ARG
} rim_result_t;

/* ============================================================
 * RawValue(受理点の型自由な生値。本物のC union)
 * ============================================================ */
typedef enum { RIM_RAW_SCALAR, RIM_RAW_BYTES } rim_raw_kind_t;

typedef struct {
    rim_raw_kind_t kind;
    union {
        double scalar;                                        /* RIM_RAW_SCALAR */
        struct { const void* data; size_t size; size_t count; } bytes; /* RIM_RAW_BYTES(非所有参照) */
    } as;
} rim_raw_value_t;

/* v は double へ暗黙変換できる型(int32_t/uint32_t/float/bool等)ならそのまま渡せる。 */
rim_raw_value_t rim_raw_scalar(double v);
/* p は呼出し中のみ有効な非所有参照(コピーしない)。elem_size は sizeof(構造体)。 */
rim_raw_value_t rim_raw_struct(const void* p, size_t elem_size);
/* p は要素数 count の配列の先頭。elem_size は sizeof(要素型)。 */
rim_raw_value_t rim_raw_array(const void* p, size_t elem_size, size_t count);

/* ============================================================
 * Context(rim::DataContext のCミラー。std::optional を has_xxx で表現)
 * ============================================================ */
typedef enum {
    RIM_FAULT_NONE = 0,
    RIM_FAULT_RAISED,
    RIM_FAULT_CLEARED,
    RIM_FAULT_ALL_CLEARED,
    RIM_FAULT_UPDATED_HEAL,
    RIM_FAULT_UPDATED_ACTIVE
} rim_fault_state_t;

/* 生値の単位/表現。同じidへ異なる単位で値が届く場合に Rule が分岐するのに使う。
 * RIM_UNIT_NORMALIZED(=既定) は「既に正規化済み。変換しない」を意味する。
 * 例) 温度は摂氏でも華氏でも投入でき、DataStore へは常にケルビンで格納される。 */
typedef enum {
    RIM_UNIT_NORMALIZED = 0,
    RIM_UNIT_CELSIUS,
    RIM_UNIT_FAHRENHEIT
} rim_source_unit_t;

typedef struct {
    bool               has_unit;
    rim_source_unit_t  unit;
    bool               has_fault_state;
    rim_fault_state_t  fault_state;
    bool               has_scale_x1000;
    int32_t            scale_x1000;
    bool               has_key;
    uint32_t           key;
} rim_context_t;

/* ============================================================
 * ライフサイクル / 受理 / 駆動
 * ============================================================ */
rim_result_t rim_init(void);
void         rim_shutdown(void);
/* driver_poll 相当は呼出し側(mainFW)の責務。1サイクル分の変化検知・配信を駆動する。 */
void         rim_dispatch(void);

/* 唯一の受理点。ctx は NULL 可(補足情報なし)。 */
rim_result_t rim_push(rim_id_t id, rim_raw_value_t raw, const rim_context_t* ctx);

/* ============================================================
 * Capability(通知)
 * ============================================================ */
typedef struct {
    bool     has_error;      bool     err_has_error;  uint32_t err_active_count;
    bool     has_job;        bool     job_active;     uint8_t  job_progress;
    bool     has_env;        bool     env_in_range;
    bool     has_maint;      bool     maint_needed;
    bool     has_health;     bool     health_healthy;
    bool     has_safety;     bool     safety_safe;
    bool     has_consumable; bool     cons_ink_low;    bool     cons_wiper_low;
    bool     has_print;      bool     print_printable;
} rim_capability_set_t;

typedef enum { RIM_PRIORITY_LOW = 0, RIM_PRIORITY_NORMAL, RIM_PRIORITY_HIGH, RIM_PRIORITY_CRITICAL } rim_capability_priority_t;
typedef enum { RIM_TRIGGER_ON_CHANGE = 0, RIM_TRIGGER_PERIODIC, RIM_TRIGGER_THRESHOLD, RIM_TRIGGER_EVENT, RIM_TRIGGER_INITIAL } rim_publish_trigger_t;

/* CapabilityKind のビット(SubscriptionSet.interested / MachineCapability.changed_kinds に使う)。 */
#define RIM_CAP_BIT_ERROR      (1u << 0)
#define RIM_CAP_BIT_JOB        (1u << 1)
#define RIM_CAP_BIT_ENV        (1u << 2)
#define RIM_CAP_BIT_MAINT      (1u << 3)
#define RIM_CAP_BIT_HEALTH     (1u << 4)
#define RIM_CAP_BIT_SAFETY     (1u << 5)
#define RIM_CAP_BIT_CONSUMABLE (1u << 6)
#define RIM_CAP_BIT_PRINT      (1u << 7)

typedef struct {
    rim_capability_set_t      capabilities;
    rim_capability_priority_t priority;
    uint32_t                  changed_kinds;
    rim_publish_trigger_t     trigger;
} rim_machine_capability_t;

typedef void (*rim_capability_cb)(const rim_machine_capability_t* cap, void* ctx);
typedef int rim_subscription_t; /* < 0 は無効ハンドル */

/* interested_kinds は RIM_CAP_BIT_* のOR、0 なら全Capabilityに関心。
 * 登録上限は RIM_SUBSCRIBER_MAX(framework の SubscriptionBroker 容量と同値)。 */
#define RIM_SUBSCRIBER_MAX 16

rim_subscription_t rim_subscribe(rim_capability_cb cb, void* ctx, uint32_t interested_kinds);
void                rim_unsubscribe(rim_subscription_t handle);

/* ============================================================
 * Accessor(参照専用・Pull)
 * ============================================================ */
#define RIM_FAULT_CAP 64  /* rim::kFaultCap と同値(framework/inc/datastore/Config.hpp) */

typedef struct {
    bool     has_data;

    bool     fault_present;
    uint32_t fault_active_count;
    uint32_t fault_active_codes[RIM_FAULT_CAP];
    uint32_t fault_healed_count;

    bool     op_present;
    bool     op_job_present;
    uint8_t  op_job_progress;
    bool     op_job_active;

    bool     cur_present;
    bool     env_present;      int32_t temperature_kelvin_x100; bool has_temperature_kelvin_x100;
                                uint8_t humidity;         bool has_humidity;
                                uint8_t pressure;         bool has_pressure;
    bool     cons_present;     uint8_t ink_level;         bool has_ink_level;
                                uint8_t wiper_level;      bool has_wiper_level;
    bool     safety_present;   bool    cover_open;        bool has_cover_open;
                                bool    e_stop;            bool has_e_stop;
    bool     maint_present;    uint32_t maint_counter;    bool has_maint_counter;
    bool     health_present;   bool    unit_alive;        bool has_unit_alive;
} rim_machine_snapshot_t;

typedef struct {
    bool                   has_data;
    rim_machine_snapshot_t data;
    bool                   has_capability;
    rim_capability_set_t   capability;
} rim_printer_status_t;

/* domains は RIM_DOMAIN_* のOR。RIM_DOMAIN_CURRENT_ALL で最新値系5領域まとめて。 */
#define RIM_DOMAIN_NONE        0u
#define RIM_DOMAIN_FAULT       (1u << 0)
#define RIM_DOMAIN_OPERATION   (1u << 1)
#define RIM_DOMAIN_ENVIRONMENT (1u << 2)
#define RIM_DOMAIN_CONSUMABLE  (1u << 3)
#define RIM_DOMAIN_SAFETY      (1u << 4)
#define RIM_DOMAIN_MAINTENANCE (1u << 5)
#define RIM_DOMAIN_HEALTH      (1u << 6)
#define RIM_DOMAIN_CURRENT_ALL (RIM_DOMAIN_ENVIRONMENT | RIM_DOMAIN_CONSUMABLE | \
                                 RIM_DOMAIN_SAFETY | RIM_DOMAIN_MAINTENANCE | RIM_DOMAIN_HEALTH)

rim_printer_status_t rim_get_status(uint32_t domains, bool include_capability);

#ifdef __cplusplus
}
#endif
