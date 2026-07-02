/**
 * @file pf_port_linux.cpp
 * @brief Linux (POSIX) 向け port 実装。
 *
 * - critical: pthread mutex（タスク/スレッド間の排他）。
 * - time:     CLOCK_MONOTONIC を ms 化。
 * - log:      stdout へ即時 flush 出力（Ubuntu の端末・TeraTerm のシリアル/SSH で閲覧可能）。
 * - assert:   stderr へ出力して abort。
 *
 * 本ファイルは Linux/POSIX 専用（pthread / clock_gettime を使用）。CMake では UNIX のときのみ
 * ビルドされ、Threads::Threads をリンクする。実機（別MCU/RTOS）では pf_port_freertos.cpp 等を使う。
 */
#include "pf_port_samples.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <pthread.h>

namespace {

pthread_mutex_t s_mtx = PTHREAD_MUTEX_INITIALIZER;   ///< critical_enter/exit が共有する唯一のミューテックス

/** @brief pthread_mutex_lock でクリティカルセクションに入る。 */
void crit_enter() { pthread_mutex_lock(&s_mtx); }
/** @brief pthread_mutex_unlock でクリティカルセクションを抜ける。 */
void crit_exit()  { pthread_mutex_unlock(&s_mtx); }

/**
 * @brief CLOCK_MONOTONIC を使って単調増加のms時刻を返す。
 *
 * 内部処理: clock_gettime() で秒(tv_sec)・ナノ秒(tv_nsec)を取得し、
 * 秒→ms（×1000）とナノ秒→ms（÷1,000,000）を足し合わせて pf_time_ms_t にまとめる。
 * 取得失敗時は 0 を返す（呼出側は「時刻が取れない」ケースとして扱う）。
 */
pf_time_ms_t time_now()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return 0u;
    return static_cast<pf_time_ms_t>(static_cast<pf_time_ms_t>(ts.tv_sec) * 1000u +
                                     static_cast<pf_time_ms_t>(ts.tv_nsec / 1000000L));
}

/**
 * @brief ログ文字列を stdout へ改行付きで出力し、即座に flush する。
 *
 * 即時flushすることで、Ubuntu端末やTeraTerm（SSH/シリアル）に接続した状態でも
 * バッファリングによる表示遅延なくリアルタイムに閲覧できるようにする。
 */
void log_out(const char* msg)
{
    if (!msg) return;
    std::fputs(msg, stdout);
    std::fputc('\n', stdout);
    std::fflush(stdout);   /* シリアル/端末に即時反映（TeraTerm でリアルタイム閲覧） */
}

/**
 * @brief assertion失敗時のハンドラ。式・ファイル名・行番号を stderr へ出力してプロセスを異常終了させる。
 *
 * 組込みの実機では abort() ではなく、独自のフェイルセーフ処理（再起動・安全停止等）に
 * 置き換えることを想定している。
 */
void assert_fail(const char* expr, const char* file, int line)
{
    std::fprintf(stderr, "[pf][ASSERT] %s at %s:%d\n", expr ? expr : "?", file ? file : "?", line);
    std::fflush(stderr);
    std::abort();
}

} // namespace

/**
 * @brief Linux(POSIX)向けの pf_port_t を組み立てて返す。
 *
 * critical はpthreadミューテックス、time_nowはCLOCK_MONOTONIC、logはstdout即時flush、
 * assert_failはstderr出力+abortで構成する（このファイル内の各staticコールバックを割り当てるだけ）。
 * @return 4種のコールバック全てが設定済みの pf_port_t。
 */
pf_port_t pf_port_linux()
{
    pf_port_t p;
    p.critical_enter = crit_enter;
    p.critical_exit  = crit_exit;
    p.time_now       = time_now;
    p.log            = log_out;
    p.assert_fail    = assert_fail;
    return p;
}
