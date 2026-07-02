/**
 * @file model_sample.c
 * @brief サンプル機種の機種依存実装（記述子表・評価器・FSM表・ドライバグルー）。
 *
 * 「温度＋フェーズ＋カバー」から抽象状態（印刷可否・温度警報・ライフサイクル）を導く例。
 * 新機種を作るときは、この1ファイル（＋ ids ヘッダ）をコピーして差分実装すればよい。コアは無改修。
 */
#include "model_sample.h"
#include "model_sample_ids.h"
#include "printer_fw/pf_data.h"

/* --- 機種依存のしきい値（モデル側の定数） --- */
#define TEMP_WARN_C   45u   /* これ以上で警告 */
#define TEMP_ALARM_C  60u   /* これ以上で異常 */
#define VOLUME_NORMAL_MIN 20   /* これ未満は静音 */
#define VOLUME_LOUD_MIN   80   /* これ以上は大音量 */

/* ============================================================
 * ① データ辞書 記述子
 * ============================================================
 * SAMPLE_RAW_VOLUME は「正規型」として PF_TYPE_I32 で登録する。
 * Aドライバ(short)・Bドライバ(long)のどちらから来た値でも、辞書に入る時点では
 * 常に i32 に統一される（変換は下のグルー関数で実施）。
 */
static const pf_data_desc_t s_data[] = {
    /* id,                 type,         elem_size, length, change_eps, name */
    { SAMPLE_RAW_TEMP,      PF_TYPE_U32,  0, 1, 0, "temp_c"  },
    { SAMPLE_RAW_PHASE,     PF_TYPE_ENUM, 0, 1, 0, "phase"   },
    { SAMPLE_RAW_DOOR_OPEN, PF_TYPE_BOOL, 0, 1, 0, "door"    },
    { SAMPLE_RAW_VOLUME,    PF_TYPE_I32,  0, 1, 0, "volume"  },
};

/* ============================================================
 * ② 状態 評価器（純関数。pf_data_get_* で生値を読む）
 * ============================================================ */

/* ライフサイクル：フェーズをそのまま状態値へ写像 */
static pf_result_t eval_main(int32_t* out, void* ctx)
{
    (void)ctx;
    int32_t phase = 0;
    pf_result_t r = pf_data_get_enum(SAMPLE_RAW_PHASE, &phase);
    if (r != PF_OK) return r;
    *out = phase;  /* SAMPLE_PHASE_* == SAMPLE_MAIN_* */
    return PF_OK;
}

/* 印刷可否：READY かつ 温度OK かつ カバー閉 のときだけ可。例「温度50+待機→印刷不可」 */
static pf_result_t eval_printable(int32_t* out, void* ctx)
{
    (void)ctx;
    uint32_t temp = 0; int32_t phase = 0; bool door = false;
    pf_result_t r;
    if ((r = pf_data_get_u32 (SAMPLE_RAW_TEMP,      &temp))  != PF_OK) return r;
    if ((r = pf_data_get_enum(SAMPLE_RAW_PHASE,     &phase)) != PF_OK) return r;
    if ((r = pf_data_get_bool(SAMPLE_RAW_DOOR_OPEN, &door))  != PF_OK) return r;

    bool ok = (phase == SAMPLE_PHASE_READY) && (temp < TEMP_WARN_C) && (!door);
    *out = ok ? SAMPLE_PRINTABLE_OK : SAMPLE_PRINTABLE_BLOCKED;
    return PF_OK;
}

/* 温度警報：しきい値で 正常/警告/異常 を判定。例「温度50→警告」「温度65→異常」 */
static pf_result_t eval_temp_alert(int32_t* out, void* ctx)
{
    (void)ctx;
    uint32_t temp = 0;
    pf_result_t r = pf_data_get_u32(SAMPLE_RAW_TEMP, &temp);
    if (r != PF_OK) return r;

    if      (temp >= TEMP_ALARM_C) *out = SAMPLE_TEMP_ALARM;
    else if (temp >= TEMP_WARN_C)  *out = SAMPLE_TEMP_WARN;
    else                           *out = SAMPLE_TEMP_NORMAL;
    return PF_OK;
}

/*
 * 音量レベル：SAMPLE_RAW_VOLUME（正規型 i32）をしきい値判定するだけ。
 * ドライバのネイティブ型が short でも long でも、この評価器は一切変更不要
 * （辞書に入った時点で型は統一されているため）。
 */
static pf_result_t eval_volume_level(int32_t* out, void* ctx)
{
    (void)ctx;
    int32_t volume = 0;
    pf_result_t r = pf_data_get_i32(SAMPLE_RAW_VOLUME, &volume);
    if (r != PF_OK) return r;

    if      (volume >= VOLUME_LOUD_MIN)   *out = SAMPLE_VOLUME_LOUD;
    else if (volume >= VOLUME_NORMAL_MIN) *out = SAMPLE_VOLUME_NORMAL;
    else                                  *out = SAMPLE_VOLUME_QUIET;
    return PF_OK;
}

static const pf_state_desc_t s_states[] = {
    /* id,                    eval,             ctx,  initial,             name */
    { SAMPLE_STATE_MAIN,         eval_main,         NULL, SAMPLE_MAIN_READY, "main"         },
    { SAMPLE_STATE_PRINTABLE,    eval_printable,    NULL, -1,                "printable"    }, /* -1=未知→初回必ず通知 */
    { SAMPLE_STATE_TEMP_ALERT,   eval_temp_alert,   NULL, -1,                "temp_alert"   },
    { SAMPLE_STATE_VOLUME_LEVEL, eval_volume_level, NULL, -1,                "volume_level" },
};

/* ============================================================
 * ③ FSM（主系ライフサイクルの許可遷移）
 * ============================================================ */
static const pf_fsm_transition_t s_main_trans[] = {
    { SAMPLE_MAIN_READY,    SAMPLE_MAIN_PRINTING },
    { SAMPLE_MAIN_PRINTING, SAMPLE_MAIN_READY    },
    { SAMPLE_MAIN_PRINTING, SAMPLE_MAIN_ERROR    },
    { SAMPLE_MAIN_READY,    SAMPLE_MAIN_ERROR    },
    { SAMPLE_MAIN_ERROR,    SAMPLE_MAIN_RECOVERY },
    { SAMPLE_MAIN_RECOVERY, SAMPLE_MAIN_READY    },
    { SAMPLE_MAIN_RECOVERY, SAMPLE_MAIN_ERROR    },
    /* 注: ERROR→PRINTING は意図的に不許可（必ず RECOVERY を経由） */
};

static const pf_fsm_desc_t s_fsm[] = {
    { SAMPLE_STATE_MAIN, SAMPLE_MAIN_READY, s_main_trans,
      sizeof s_main_trans / sizeof s_main_trans[0], "main_fsm" },
};

/* ============================================================
 * ④ ドライバグルー（任意）
 * ============================================================
 * 実機では driver_poll で ADC/GPIO 等を読み、pf_data_set_* で辞書へ書き込む。
 * 本サンプルではデモ側が辞書を直接更新するため driver_init/poll は NULL とする。
 *
 * 音量まわりは「機種によりドライバのネイティブ型が異なる」ケースの実例として、
 * 型変換グルーだけを個別に用意する（Aドライバ=short / Bドライバ=long）。
 * どちらを呼んでも、辞書に入るのは常に SAMPLE_RAW_VOLUME(i32) の同じ値になる。
 */
pf_result_t model_sample_volume_ingest_from_short(short native_value)
{
    /* short(通常16bit) → i32 は無損失の符号拡張 */
    return pf_data_set_i32(SAMPLE_RAW_VOLUME, (int32_t)native_value);
}

pf_result_t model_sample_volume_ingest_from_long(long native_value)
{
    /* long は環境依存(32/64bit)。この機種は long も 32bit の値域で運用する前提のため
     * i32 へキャストする。64bit long 環境かつ32bitを超える値を扱う場合は、正規型を
     * 拡張する（例: PF_TYPE を追加）などの見直しが必要になる点に注意。 */
    return pf_data_set_i32(SAMPLE_RAW_VOLUME, (int32_t)native_value);
}

/* ============================================================
 * モデル記述子
 * ============================================================ */
static const pf_model_t s_model = {
    "model_sample",
    s_data,   sizeof s_data   / sizeof s_data[0],
    s_states, sizeof s_states / sizeof s_states[0],
    s_fsm,    sizeof s_fsm    / sizeof s_fsm[0],
    NULL,  /* driver_init */
    NULL   /* driver_poll */
};

const pf_model_t* model_sample_get(void)
{
    return &s_model;
}
