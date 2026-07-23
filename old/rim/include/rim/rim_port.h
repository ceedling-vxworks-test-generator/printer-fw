/*
 * rim_port.h - プラットフォーム排他ポート（C ABI・H-7）
 *
 * DataStore の投入(post)・取り込み(dispatch)・Snapshot取得(capture) を、
 * 多数のRTOSタスクから安全に呼べるようにするための短いクリティカルセクション境界。
 *   - 生産者はタスクのみ（ISRなし）想定 → 単一の相互排他で十分（ブロッキング可）。
 *   - 本ヘッダは2関数のseam。既定実装(src/rim_port.cpp)はno-op(weak)で、
 *     実機（例: VxWorks の semMCreate/semTake/semGive）で上書きできる。
 *   - 再入しない前提（dispatchは notify_updated 発火前にunlockする）。
 */
#ifndef RIM_PORT_H
#define RIM_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* RIM共通の相互排他を取得/解放する。lock→…→unlock は必ず対で呼ぶこと。 */
void rim_port_lock(void);
void rim_port_unlock(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RIM_PORT_H */
