/*
 * rim_capi_perf_test.c - capi の性能測定。
 *
 * 方針: 数値は毎回ログへ出力し、アサートは「桁が変わるような破滅的退行」だけを捕まえる
 * 非常に緩い上限に留める。CI環境の負荷で揺れる厳しい閾値は偽陽性の温床になるため。
 * 実機の性能保証はターゲット上での計測で行うこと(本測定はホスト上の相対比較用)。
 *
 * 時間計測は ISO C の clock() を使う(clock_gettime は POSIX 依存のため、
 * ベアメタル/RTOS ターゲットへ持ち込めない)。CPU時間ベース・分解能は CLOCKS_PER_SEC。
 */

#include "rim_capi_test.h"
#include "rim_capi.h"

#include <time.h>
#include <stdint.h>

#ifndef RIM_PERF_ITERATIONS
#define RIM_PERF_ITERATIONS 200000
#endif

/* 破滅的退行の検出のみを目的とした緩い上限(ns/op)。 */
#define RIM_PERF_LIMIT_NS_PUSH        50000.0   /* 実測 ~45ns に対し3桁の余裕 */
#define RIM_PERF_LIMIT_NS_DISPATCH   200000.0
#define RIM_PERF_LIMIT_NS_STATUS     200000.0

static double NsPerOp(clock_t begin, clock_t end, long iterations)
{
    const double sec = (double)(end - begin) / (double)CLOCKS_PER_SEC;
    return (iterations > 0) ? (sec * 1e9 / (double)iterations) : 0.0;
}

static void Report(const char* label, double ns_per_op, double limit_ns)
{
    printf("  [ PERF     ] %-22s %10.1f ns/op  (上限 %.0f)\n", label, ns_per_op, limit_ns);
    if (ns_per_op > limit_ns) {
        RIM_FAIL_("%s が上限を超過: %.1f ns/op > %.1f ns/op", label, ns_per_op, limit_ns);
    }
}

/* 受理のみ(Adapter→Rule変換→L2の分類→バッファ格納)。 */
RIM_TEST(PerfPushOnly)
{
    const long n = RIM_PERF_ITERATIONS;
    const clock_t t0 = clock();
    for (long i = 0; i < n; ++i) {
        /* 毎回異なる値にして「変化なしで早期returnする」経路に偏らせない。 */
        rim_push(RIM_ID_TEMPERATURE, rim_raw_scalar(20.0 + (double)(i % 100) * 0.01), NULL);
    }
    const clock_t t1 = clock();
    Report("rim_push", NsPerOp(t0, t1, n), RIM_PERF_LIMIT_NS_PUSH);
}

/* 受理 + 1サイクル駆動(Registry反映・差分判定・Capability生成・配信まで)。 */
RIM_TEST(PerfPushAndDispatch)
{
    const long n = RIM_PERF_ITERATIONS;
    const clock_t t0 = clock();
    for (long i = 0; i < n; ++i) {
        rim_push(RIM_ID_TEMPERATURE, rim_raw_scalar(20.0 + (double)(i % 100) * 0.01), NULL);
        rim_dispatch();
    }
    const clock_t t1 = clock();
    Report("rim_push+rim_dispatch", NsPerOp(t0, t1, n), RIM_PERF_LIMIT_NS_DISPATCH);
}

/* Accessor の Pull 参照(全ドメイン + Capability)。戻り値は値返しでサイズが大きい点に注意。 */
RIM_TEST(PerfGetStatusAllDomains)
{
    const long n = RIM_PERF_ITERATIONS / 10;
    volatile uint32_t sink = 0;   /* 最適化で呼び出しごと消されないように使う */
    const clock_t t0 = clock();
    for (long i = 0; i < n; ++i) {
        rim_printer_status_t st = rim_get_status(RIM_DOMAIN_CURRENT_ALL, true);
        sink += st.data.maint_counter;
    }
    const clock_t t1 = clock();
    (void)sink;
    Report("rim_get_status(全域)", NsPerOp(t0, t1, n), RIM_PERF_LIMIT_NS_STATUS);

    /* 戻り値のサイズを記録する。呼び出しごとにこのサイズのコピーが発生するため、
     * 高頻度ポーリングする用途では無視できないコストになる。 */
    printf("  [ INFO     ] sizeof(rim_printer_status_t) = %u bytes\n",
           (unsigned)sizeof(rim_printer_status_t));
}

/* --- 購読者ありの配信コスト --- */
static int g_perf_cb_hits = 0;
static void PerfOnPublish(const rim_machine_capability_t* cap, void* ctx)
{
    (void)cap; (void)ctx;
    ++g_perf_cb_hits;
}

/*
 * 定常状態(生値は動くが Capability は変わらない)のコスト。
 * RIM は「生値の変化」ではなく「Capability(意味づけ結果)の変化」で配信するため、
 * 20.00〜20.99℃ の範囲で値が動いても EnvCap.inRange は true のままで配信は起きない。
 * これが実運用で最も多い経路であり、購読者がいても配信されないことを併せて確認する。
 */
RIM_TEST(PerfDispatchWithSubscriberNoCapabilityChange)
{
    const rim_subscription_t sub = rim_subscribe(PerfOnPublish, NULL, 0);
    RIM_EXPECT(sub >= 0);

    /* 先に1回流して Capability を確定させる(初回はInitial配信が起きるため計測から除く)。 */
    rim_push(RIM_ID_TEMPERATURE, rim_raw_scalar(20.0), NULL);
    rim_dispatch();

    const long n = RIM_PERF_ITERATIONS;
    g_perf_cb_hits = 0;
    const clock_t t0 = clock();
    for (long i = 0; i < n; ++i) {
        rim_push(RIM_ID_TEMPERATURE, rim_raw_scalar(20.0 + (double)(i % 100) * 0.01), NULL);
        rim_dispatch();
    }
    const clock_t t1 = clock();
    Report("配信なし(定常)", NsPerOp(t0, t1, n), RIM_PERF_LIMIT_NS_DISPATCH);

    /* Capability が変わらない限り配信されない(帯域を消費しない)という設計契約の確認。 */
    printf("  [ INFO     ] コールバック呼出回数 = %d / %ld 回のdispatch\n", g_perf_cb_hits, n);
    RIM_EXPECT_EQ_I(g_perf_cb_hits, 0);

    rim_unsubscribe(sub);
}

/*
 * 最悪ケース: 毎サイクル Capability が変化し、毎回配信＋コールバックまで走る。
 * 温度の警告閾値(60.00℃)を挟んで 20℃ ↔ 70℃ を往復させ、EnvCap.inRange を毎回反転させる。
 */
RIM_TEST(PerfDispatchWithPublishEveryCycle)
{
    const rim_subscription_t sub = rim_subscribe(PerfOnPublish, NULL, 0);
    RIM_EXPECT(sub >= 0);

    const long n = RIM_PERF_ITERATIONS;
    g_perf_cb_hits = 0;
    const clock_t t0 = clock();
    for (long i = 0; i < n; ++i) {
        rim_push(RIM_ID_TEMPERATURE, rim_raw_scalar((i % 2) ? 70.0 : 20.0), NULL);
        rim_dispatch();
    }
    const clock_t t1 = clock();
    Report("毎回配信(最悪)", NsPerOp(t0, t1, n), RIM_PERF_LIMIT_NS_DISPATCH);
    printf("  [ INFO     ] コールバック呼出回数 = %d / %ld 回のdispatch\n", g_perf_cb_hits, n);

    /* 閾値を跨いでいるので配信が発生すること。 */
    RIM_EXPECT(g_perf_cb_hits > 0);
    RIM_EXPECT(g_perf_cb_hits <= n);

    rim_unsubscribe(sub);

    /* 後続へ影響を残さないよう範囲内へ戻す。 */
    rim_push(RIM_ID_TEMPERATURE, rim_raw_scalar(20.0), NULL);
    rim_dispatch();
}

void RimCapiPerfTests(void)
{
    RIM_RUN(PerfPushOnly);
    RIM_RUN(PerfPushAndDispatch);
    RIM_RUN(PerfGetStatusAllDomains);
    RIM_RUN(PerfDispatchWithSubscriberNoCapabilityChange);
    RIM_RUN(PerfDispatchWithPublishEveryCycle);
}
