/**
 * @file model_sample_ids.h
 * @brief サンプル機種のデータID・状態ID・状態値の定義（機種依存）。
 *
 * 新機種を作るときは、このファイルを基にIDを定義し直す。IDは 0..N-1 の稠密値にする。
 */
#ifndef MODEL_SAMPLE_IDS_H
#define MODEL_SAMPLE_IDS_H

/* --- Raw データID（辞書のキー）--- */
enum {
    SAMPLE_RAW_TEMP = 0,   /* u32: 温度[℃] */
    SAMPLE_RAW_PHASE,      /* enum: 動作フェーズ（下記 phase 値） */
    SAMPLE_RAW_DOOR_OPEN,  /* bool: カバー開 */
    SAMPLE_RAW_VOLUME,     /* i32: 音量（正規型）。Aドライバ(short)/Bドライバ(long)等、
                              機種ごとにネイティブ型が異なっても、辞書には常にこの正規型(i32)で
                              格納する。変換は各機種のドライバグルー側で行う（コア無改修）。 */
    SAMPLE_RAW_COUNT       /* 番兵：データ数 */
};

/* SAMPLE_RAW_PHASE の取り得る値（ドライバが設定） */
enum {
    SAMPLE_PHASE_READY = 0,
    SAMPLE_PHASE_PRINTING,
    SAMPLE_PHASE_ERROR,
    SAMPLE_PHASE_RECOVERY
};

/* --- 抽象状態ID --- */
enum {
    SAMPLE_STATE_MAIN = 0,      /* ライフサイクル（FSM管理）: READY/PRINTING/ERROR/RECOVERY */
    SAMPLE_STATE_PRINTABLE,     /* 印刷可否: 0=可 / 1=不可 */
    SAMPLE_STATE_TEMP_ALERT,    /* 温度警報: 0=正常 / 1=警告 / 2=異常 */
    SAMPLE_STATE_VOLUME_LEVEL,  /* 音量レベル: 0=静音 / 1=通常 / 2=大音量 */
    SAMPLE_STATE_COUNT          /* 番兵：状態数 */
};

/* SAMPLE_STATE_MAIN の値（= phase と1:1） */
enum {
    SAMPLE_MAIN_READY = 0,
    SAMPLE_MAIN_PRINTING,
    SAMPLE_MAIN_ERROR,
    SAMPLE_MAIN_RECOVERY
};

/* SAMPLE_STATE_PRINTABLE の値 */
enum { SAMPLE_PRINTABLE_OK = 0, SAMPLE_PRINTABLE_BLOCKED = 1 };

/* SAMPLE_STATE_TEMP_ALERT の値 */
enum { SAMPLE_TEMP_NORMAL = 0, SAMPLE_TEMP_WARN = 1, SAMPLE_TEMP_ALARM = 2 };

/* SAMPLE_STATE_VOLUME_LEVEL の値 */
enum { SAMPLE_VOLUME_QUIET = 0, SAMPLE_VOLUME_NORMAL = 1, SAMPLE_VOLUME_LOUD = 2 };

#endif /* MODEL_SAMPLE_IDS_H */
