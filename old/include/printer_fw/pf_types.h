/**
 * @file pf_types.h
 * @brief printer-fw 共通の基本型・値表現。型非依存データ授受の核。
 */
#ifndef PRINTER_FW_PF_TYPES_H
#define PRINTER_FW_PF_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t pf_data_id_t;   /**< データ辞書のID（0..N-1 の稠密値を基本とする） */
typedef uint16_t pf_state_id_t;  /**< 抽象状態のID（同上）。0xFFFF は予約(PF_STATE_ANY) */
typedef uint32_t pf_time_ms_t;   /**< 単調増加のms時刻 */

/** 辞書セルの型タグ。 */
typedef enum {
    PF_TYPE_BOOL = 0,
    PF_TYPE_U8,
    PF_TYPE_U16,
    PF_TYPE_U32,
    PF_TYPE_I32,
    PF_TYPE_F32,
    PF_TYPE_F64,
    PF_TYPE_ENUM,   /**< int32 として保持 */
    PF_TYPE_BLOB    /**< 構造体 / 配列 / 可変長 */
} pf_type_t;

/**
 * タグ付き値。型を問わない授受に使う。
 * 状態値は別途 int32_t に正規化して扱う（Monitor の比較を単純・高速にするため）。
 */
typedef struct {
    pf_type_t type;
    union {
        bool     b;
        uint8_t  u8;
        uint16_t u16;
        uint32_t u32;
        int32_t  i32;
        float    f32;
        double   f64;
        int32_t  e;   /**< enum */
        struct {
            const void* ptr;  /**< BLOB はプール内へのゼロコピー参照（寿命注意） */
            uint16_t    size;
        } blob;
    } as;
} pf_value_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRINTER_FW_PF_TYPES_H */
