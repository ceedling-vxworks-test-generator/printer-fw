/*
 * rim_port.cpp - プラットフォーム排他ポートの既定実装（no-op・weak）
 *
 * スケルトン/単体テスト用の既定は「何もしない」。多数タスクから使う実機では、
 * このシンボルをプラットフォーム版で上書きする（weak属性で差し替え可能）。
 *   - 例(VxWorks): SEM_ID mutex = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);
 *                  rim_port_lock   → semTake(mutex, WAIT_FOREVER);
 *                  rim_port_unlock → semGive(mutex);
 *   - 生産者はタスクのみ（ISRなし）想定のため、単一mutexで直列化すれば足りる。
 *
 * 注: weak 属性は GCC/Clang/VxWorks(diab/gnu) で利用可能。非対応時は本ファイルを
 *     プラットフォーム実装で置換すればよい。
 */
#include "rim/rim_port.h"

extern "C" {

__attribute__((weak)) void rim_port_lock(void)   { /* 既定: no-op */ }
__attribute__((weak)) void rim_port_unlock(void) { /* 既定: no-op */ }

} /* extern "C" */
