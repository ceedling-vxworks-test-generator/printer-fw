/*
 * rim_capi_perf_test.c - capi の性能測定。
 *
 * 方針: 数値は毎回ログへ出力し、アサートは「桁が変わるような破滅的退行」だけを
 * 捕まえる非常に緩い上限に留める。CI環境の負荷で揺れる厳しい閾値は偽陽性の
 * 温床になるため。実機の性能保証はターゲット上での計測で行うこと(本測定は
 * ホスト上の相対比較用)。
 *
 * 時間計測は ISO C の clock() を使う(clock_gettime は POSIX 依存のため、
 * ベアメタル/RTOS ターゲットへ持ち込めない)。CPU時間ベース・分解能は
 * CLOCKS_PER_SEC。
 *
 * 旧実装との違い: 旧 capi は呼出側が rim_dispatch() を明示的に呼ぶ同期モデル
 * だったため「push だけ」「push+dispatch」を別々に計測できた。この実装は
 * RIManager_Start() で起動したワーカスレッドが背後で非同期に処理するため、
 * 「push だけ」の計測はそのまま対応するが、「dispatch」に対応する操作が無い。
 * その代わりとして、末尾の値が Capability へ反映されるまでの
 * 実測待ち時間(エンドツーエンド遅延)を別途計測する。
 */

#include "rim_capi_test.h"
#include "rim_capi_test_helpers.h"

#include <time.h>
#include <stdint.h>

#ifndef RIM_PERF_ITERATIONS
#define RIM_PERF_ITERATIONS 200000
#endif

/* 破滅的退行の検出のみを目的とした緩い上限(ns/op)。 */
#define RIM_PERF_LIMIT_NS_PUSH        50000.0   /* 実測 ~数十ns に対し3桁の余裕 */
#define RIM_PERF_LIMIT_NS_GET_CURRENT 200000.0

static double NsPerOp(clock_t begin, clock_t end, long iterations)
{
    const double sec = (double)(end - begin) / (double)CLOCKS_PER_SEC;
    return (iterations > 0) ? (sec * 1e9 / (double)iterations) : 0.0;
}

static void Report(const char* label, double ns_per_op, double limit_ns)
{
    printf("  [ PERF     ] %-28s %10.1f ns/op  (上限 %.0f)\n", label, ns_per_op, limit_ns);
    if (ns_per_op > limit_ns) {
        RIM_FAIL_("%s が上限を超過: %.1f ns/op > %.1f ns/op", label, ns_per_op, limit_ns);
    }
}

/* 受理のみ(Adapter -> Rule変換 -> Datastoreキューへの投入)。
 * ワーカスレッドの処理完了は待たない(旧 PerfPushOnly に対応)。 */
RIM_TEST(PerfPushOnly)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    const long n = RIM_PERF_ITERATIONS;
    const clock_t t0 = clock();
    for (long i = 0; i < n; ++i) {
        /* 毎回異なる値にして「変化なしで早期returnする」経路に偏らせない。 */
        RimPush(RIM_ID_TEMPERATURE_SENSOR_A, 293.15 + (double)(i % 100) * 0.01);
    }
    const clock_t t1 = clock();
    Report("RIManager_Push", NsPerOp(t0, t1, n), RIM_PERF_LIMIT_NS_PUSH);

    RIManager_Stop();
    RIManager_Destroy();
}

/* 現在値の読み出し(RIManager_GetCurrentCapability)の呼び出しコスト。
 * 戻り値は引数経由のコピーであり、旧実装(値返し)より小さいはずである。 */
RIM_TEST(PerfGetCurrentCapability)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    rim_job_capability_t job;
    RIM_EXPECT(RimPushAndWaitCurrentNoContext(RIM_ID_JOB_ID, 1.0, RIM_CAP_JOB, &job, sizeof job));

    const long n = RIM_PERF_ITERATIONS;
    volatile int32_t sink = 0;   /* 最適化で呼び出しごと消されないように使う */
    const clock_t t0 = clock();
    for (long i = 0; i < n; ++i) {
        RIManager_GetCurrentCapability(RIM_CAP_JOB, &job, sizeof job, NULL);
        sink += job.jobId;
    }
    const clock_t t1 = clock();
    (void)sink;
    Report("RIManager_GetCurrentCapability", NsPerOp(t0, t1, n), RIM_PERF_LIMIT_NS_GET_CURRENT);

    printf("  [ INFO     ] sizeof(rim_job_capability_t) = %u bytes\n", (unsigned)sizeof job);

    RIManager_Stop();
    RIManager_Destroy();
}

/*
 * 定常状態(生値は動くが Capability は変わらない)では配信が起きないこと。
 * RIM は「生値の変化」ではなく「Capability(意味づけ結果)の変化」で配信するため、
 * PrintReady が変わらない範囲(扉は開閉しない)で他の値を動かしても配信は
 * 起きない。これが実運用で最も多い経路であり、購読者がいても配信されない
 * ことを確認する(旧 PerfDispatchWithSubscriberNoCapabilityChange に対応)。
 */
static int g_perf_cb_hits = 0;
static void PerfOnPublish(uint64_t subscriptionId, RICapabilityId capabilityId,
                           const void* data, size_t size, void* userData)
{
    (void)subscriptionId; (void)capabilityId; (void)data; (void)size; (void)userData;
    ++g_perf_cb_hits;
}

RIM_TEST(PerfSteadyStateDoesNotPublish)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    uint64_t sub = 0;
    RIM_EXPECT_EQ_I(
        RIManager_SubscribeCapability(RIM_CAP_PRINT_READY, PerfOnPublish, NULL, &sub),
        RI_SUCCESS);

    /* 先に1回流して Capability を確定させる(初回は必ず配信されるため計測から除く)。 */
    RimPush(RIM_ID_UPPER_DOOR_OPEN, 0.0);
    RIM_EXPECT(RimWaitPending(RIM_CAP_PRINT_READY));

    const long n = RIM_PERF_ITERATIONS / 100;
    g_perf_cb_hits = 0;
    for (long i = 0; i < n; ++i) {
        /* PrintReady に無関係な温度を動かしても PrintReady 自体は変化しない。 */
        RimPush(RIM_ID_TEMPERATURE_SENSOR_A, 293.15 + (double)(i % 100) * 0.01);
    }
    /* 「配信が起きない」ことを確認したいので、ここでは伝播完了を待つ手段が無い
     * (待てるのは「起きたこと」だけ)。十分な余裕を持って空費やしする。 */
    RimSettle();

    printf("  [ INFO     ] コールバック呼出回数 = %d / %ld 回の push\n", g_perf_cb_hits, n);
    RIM_EXPECT_EQ_I(g_perf_cb_hits, 0);

    RIManager_Unsubscribe(sub);
    RIManager_Stop();
    RIManager_Destroy();
}

/*
 * 最悪ケース: 毎回 Capability が変化し、毎回配信 + コールバックまで走る。
 * 扉を開閉させて PrintReady.ready を毎回反転させる
 * (旧 PerfDispatchWithPublishEveryCycle に対応)。
 */
RIM_TEST(PerfEveryChangePublishes)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    uint64_t sub = 0;
    RIM_EXPECT_EQ_I(
        RIManager_SubscribeCapability(RIM_CAP_PRINT_READY, PerfOnPublish, NULL, &sub),
        RI_SUCCESS);

    const long n = RIM_PERF_ITERATIONS / 100;
    g_perf_cb_hits = 0;
    for (long i = 0; i < n; ++i) {
        RimPush(RIM_ID_UPPER_DOOR_OPEN, (i % 2) ? 1.0 : 0.0);
    }
    RIM_EXPECT(RimWaitPending(RIM_CAP_PRINT_READY) || g_perf_cb_hits > 0);

    printf("  [ INFO     ] コールバック呼出回数 = %d / %ld 回の push\n", g_perf_cb_hits, n);

    /* 毎回反転しているので配信が発生すること。 */
    RIM_EXPECT(g_perf_cb_hits > 0);

    RIManager_Unsubscribe(sub);

    /* 後続へ影響を残さないよう閉扉へ戻す。 */
    RimPush(RIM_ID_UPPER_DOOR_OPEN, 0.0);
    RimSettle();

    RIManager_Stop();
    RIManager_Destroy();
}

void RimCapiPerfTests(void)
{
    RIM_RUN(PerfPushOnly);
    RIM_RUN(PerfGetCurrentCapability);
    RIM_RUN(PerfSteadyStateDoesNotPublish);
    RIM_RUN(PerfEveryChangePublishes);
}
