/**
 * @file model_sample.cpp
 * @brief サンプル機種の機種依存実装（記述子表・評価器・FSM表・ドライバグルー）。
 *
 * 「温度＋フェーズ＋カバー」から抽象状態（印刷可否・温度警報・ライフサイクル）を導く例。
 * 新機種を作るときは、この1ファイル（＋ ids ヘッダ）をコピーして差分実装すればよい。コアは無改修。
 *
 * 評価器・グルー関数は状態を持たない（記述子表への登録・辞書アクセスのみ）ため、
 * クラス化はせずファイルスコープの関数として実装する。
 */
#include "model_sample.h"
#include "model_sample_ids.h"
#include "printer_fw/pf_data.h"

namespace {

/* --- 機種依存のしきい値（モデル側の定数） --- */
constexpr uint32_t kTempWarnC     = 45;   /* これ以上で警告 */
constexpr uint32_t kTempAlarmC    = 60;   /* これ以上で異常 */
constexpr int32_t  kVolumeNormalMin = 20; /* これ未満は静音 */
constexpr int32_t  kVolumeLoudMin   = 80; /* これ以上は大音量 */

/* ============================================================
 * ① データ辞書 記述子
 * ============================================================
 * SAMPLE_RAW_VOLUME は「正規型」として PF_TYPE_I32 で登録する。
 * Aドライバ(short)・Bドライバ(long)のどちらから来た値でも、辞書に入る時点では
 * 常に i32 に統一される（変換は下のグルー関数で実施）。
 */
const pf_data_desc_t s_data[] = {
    /* id,                 type,         elem_size, length, change_eps, name */
    { SAMPLE_RAW_TEMP,      PF_TYPE_U32,  0, 1, 0, "temp_c"  },
    { SAMPLE_RAW_PHASE,     PF_TYPE_ENUM, 0, 1, 0, "phase"   },
    { SAMPLE_RAW_DOOR_OPEN, PF_TYPE_BOOL, 0, 1, 0, "door"    },
    { SAMPLE_RAW_VOLUME,    PF_TYPE_I32,  0, 1, 0, "volume"  },
};

/* ============================================================
 * ② 状態 評価器（純関数。pf_data_get_* で生値を読む）
 * ============================================================ */

/**
 * @brief ライフサイクル状態の評価器：フェーズをそのまま状態値へ写像する。
 *
 * SAMPLE_PHASE_* と SAMPLE_MAIN_* は数値が1:1で一致するよう定義されているため、
 * 変換処理は不要で、読み取った値をそのまま出力するだけでよい。
 * @param out 出力先（状態値）。ctxは本評価器では未使用。
 * @return 生データ読み取りの結果（pf_data_get_enum の戻り値）。
 */
pf_result_t eval_main(int32_t* out, void* ctx)
{
    (void)ctx;
    int32_t phase = 0;
    pf_result_t r = pf_data_get_enum(SAMPLE_RAW_PHASE, &phase);
    if (r != PF_OK) return r;
    *out = phase;  /* SAMPLE_PHASE_* == SAMPLE_MAIN_* */
    return PF_OK;
}

/**
 * @brief 印刷可否の評価器：READY かつ 温度OK かつ カバー閉 のときだけ「印刷可」。
 *
 * 内部処理: 温度・フェーズ・カバー状態の3つの生データを読み取り、
 * 「フェーズがREADY」「温度がしきい値未満」「カバーが閉じている」の
 * 全条件を満たすかどうかで印刷可否を判定する（例: 温度50℃+待機中→印刷不可）。
 * いずれかの読み取りに失敗した場合は、その時点でエラーを返して中断する。
 */
pf_result_t eval_printable(int32_t* out, void* ctx)
{
    (void)ctx;
    uint32_t temp = 0; int32_t phase = 0; bool door = false;
    pf_result_t r;
    if ((r = pf_data_get_u32 (SAMPLE_RAW_TEMP,      &temp))  != PF_OK) return r;
    if ((r = pf_data_get_enum(SAMPLE_RAW_PHASE,     &phase)) != PF_OK) return r;
    if ((r = pf_data_get_bool(SAMPLE_RAW_DOOR_OPEN, &door))  != PF_OK) return r;

    bool ok = (phase == SAMPLE_PHASE_READY) && (temp < kTempWarnC) && (!door);
    *out = ok ? SAMPLE_PRINTABLE_OK : SAMPLE_PRINTABLE_BLOCKED;
    return PF_OK;
}

/**
 * @brief 温度警報の評価器：しきい値で 正常/警告/異常 の3段階に分類する。
 *
 * 内部処理: 温度を読み取り、高いほうから順に判定する
 * （kTempAlarmC以上→異常、kTempWarnC以上→警告、それ未満→正常。例: 50→警告、65→異常）。
 */
pf_result_t eval_temp_alert(int32_t* out, void* ctx)
{
    (void)ctx;
    uint32_t temp = 0;
    pf_result_t r = pf_data_get_u32(SAMPLE_RAW_TEMP, &temp);
    if (r != PF_OK) return r;

    if      (temp >= kTempAlarmC) *out = SAMPLE_TEMP_ALARM;
    else if (temp >= kTempWarnC)  *out = SAMPLE_TEMP_WARN;
    else                          *out = SAMPLE_TEMP_NORMAL;
    return PF_OK;
}

/**
 * @brief 音量レベルの評価器：SAMPLE_RAW_VOLUME（正規型 i32）をしきい値判定するだけ。
 *
 * ドライバのネイティブ型が short でも long でも、この評価器は一切変更不要
 * （辞書に入った時点で型は統一されているため）。判定ロジック自体は
 * eval_temp_alert と同様、高い方から順にしきい値と比較する。
 */
pf_result_t eval_volume_level(int32_t* out, void* ctx)
{
    (void)ctx;
    int32_t volume = 0;
    pf_result_t r = pf_data_get_i32(SAMPLE_RAW_VOLUME, &volume);
    if (r != PF_OK) return r;

    if      (volume >= kVolumeLoudMin)   *out = SAMPLE_VOLUME_LOUD;
    else if (volume >= kVolumeNormalMin) *out = SAMPLE_VOLUME_NORMAL;
    else                                 *out = SAMPLE_VOLUME_QUIET;
    return PF_OK;
}

const pf_state_desc_t s_states[] = {
    /* id,                    eval,             ctx,     initial,             name */
    { SAMPLE_STATE_MAIN,         eval_main,         nullptr, SAMPLE_MAIN_READY, "main"         },
    { SAMPLE_STATE_PRINTABLE,    eval_printable,    nullptr, -1,                "printable"    }, /* -1=未知→初回必ず通知 */
    { SAMPLE_STATE_TEMP_ALERT,   eval_temp_alert,   nullptr, -1,                "temp_alert"   },
    { SAMPLE_STATE_VOLUME_LEVEL, eval_volume_level, nullptr, -1,                "volume_level" },
};

/* ============================================================
 * ③ FSM（主系ライフサイクルの許可遷移）
 * ============================================================ */
const pf_fsm_transition_t s_main_trans[] = {
    { SAMPLE_MAIN_READY,    SAMPLE_MAIN_PRINTING },
    { SAMPLE_MAIN_PRINTING, SAMPLE_MAIN_READY    },
    { SAMPLE_MAIN_PRINTING, SAMPLE_MAIN_ERROR    },
    { SAMPLE_MAIN_READY,    SAMPLE_MAIN_ERROR    },
    { SAMPLE_MAIN_ERROR,    SAMPLE_MAIN_RECOVERY },
    { SAMPLE_MAIN_RECOVERY, SAMPLE_MAIN_READY    },
    { SAMPLE_MAIN_RECOVERY, SAMPLE_MAIN_ERROR    },
    /* 注: ERROR→PRINTING は意図的に不許可（必ず RECOVERY を経由） */
};

const pf_fsm_desc_t s_fsm[] = {
    { SAMPLE_STATE_MAIN, SAMPLE_MAIN_READY, s_main_trans,
      sizeof s_main_trans / sizeof s_main_trans[0], "main_fsm" },
};

/* ============================================================
 * ④ ドライバグルー（任意）
 * ============================================================
 * 実機では driver_poll で ADC/GPIO 等を読み、pf_data_set_* で辞書へ書き込む。
 * 本サンプルではデモ側が辞書を直接更新するため driver_init/poll は NULL とする。
 */

/* ============================================================
 * モデル記述子
 * ============================================================ */
const pf_model_t s_model = {
    "model_sample",
    s_data,   sizeof s_data   / sizeof s_data[0],
    s_states, sizeof s_states / sizeof s_states[0],
    s_fsm,    sizeof s_fsm    / sizeof s_fsm[0],
    nullptr,  /* driver_init: 本サンプルではデモ側が辞書を直接更新するため未使用 */
    nullptr   /* driver_poll */
};

} // namespace

/**
 * @brief サンプル機種の記述子(pf_model_t)を返す。
 *
 * この1関数を core_init() に渡すだけで、上記の全記述子（データ辞書・状態・FSM）が
 * まとめて登録される。新機種を追加する場合は、この関数と同名の model_xxx_get() を
 * 実装し、対応する s_data/s_states/s_fsm を用意すればよい。
 * @return モデル記述子への静的なポインタ（プログラム終了まで有効）。
 */
const pf_model_t* model_sample_get()
{
    return &s_model;
}

/* ============================================================
 * ④ ドライバグルー：ネイティブ型→正規型(i32)への取り込み例
 * ============================================================
 * 機種によりドライバのネイティブ型が異なっても（例: Aドライバ=short, Bドライバ=long）、
 * 辞書 (pf_data) には常に SAMPLE_RAW_VOLUME を i32 として格納する。型の違いを吸収する
 * 変換処理は、このようにモデル側のグルー関数だけに閉じ込め、core・評価器は一切変更しない。
 */

/**
 * @brief Aドライバ（音量をshortで保持する機種）からの取り込みグルー。
 *
 * short(通常16bit)からi32への符号拡張は情報の欠落がなく無損失であるため、
 * 単純なキャストのみで辞書の正規型へ変換できる。
 * @param native_value Aドライバのネイティブ表現（short）の音量値。
 * @return pf_data_set_i32 の結果。
 */
pf_result_t model_sample_volume_ingest_from_short(short native_value)
{
    /* short(通常16bit) → i32 は無損失の符号拡張 */
    return pf_data_set_i32(SAMPLE_RAW_VOLUME, static_cast<int32_t>(native_value));
}

/**
 * @brief Bドライバ（音量をlongで保持する機種）からの取り込みグルー。
 *
 * long は環境依存(32/64bit)。この機種は long も 32bit の値域で運用する前提のため
 * i32 へキャストする。64bit long 環境かつ32bitを超える値を扱う場合は、正規型を
 * 拡張する（例: PF_TYPE を追加）などの見直しが必要になる点に注意。
 * @param native_value Bドライバのネイティブ表現（long）の音量値。
 * @return pf_data_set_i32 の結果。
 */
pf_result_t model_sample_volume_ingest_from_long(long native_value)
{
    return pf_data_set_i32(SAMPLE_RAW_VOLUME, static_cast<int32_t>(native_value));
}
