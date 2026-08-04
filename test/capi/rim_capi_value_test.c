/*
 * rim_capi_value_test.c - capi の「値の保証」テスト。
 *
 * 受理点が double 1本(RIManager_Push)であるため、整数値が double を経由する
 * ことによる値落ちを懸念する声があった。本ファイルはその実挙動を固定する
 * 回帰テストである。
 *
 * 結論(以前の調査 + 本ファイルで再確認):
 *   - double の仮数部は53bitあるため、int32 の全域は double を経由しても
 *     完全に正確。float(仮数部24bit)では失われる 2^24+1 のような値も
 *     double では保たれる。
 *   - 温度は本実装では**ケルビンの double のまま Capability に格納される**
 *     (旧実装は x100 の固定小数点 int32 に丸めていたため、そこでの切り捨てが
 *     値落ちの実体だった)。この実装では温度経路に整数変換が無いため、
 *     その種の切り捨ては起きない。
 *   - PercentRule/CountRule 等、整数へ変換する Rule では
 *     static_cast<int32_t> による 0 方向への切り捨てが残る
 *     (§ TruncatesTowardZero)。
 *   - 型の範囲外(int32 の範囲を大きく超える値)は規格上 UB。本環境
 *     (x86-64 / GCC)では cvttsd2si の「不正な整数」表現である INT32_MIN へ
 *     飽和することを実測で確認した。他環境への移植時は変わりうるため、
 *     この挙動に依存してはならない(§ OutOfRangeIsUnchecked)。
 */

#include "rim_capi_test.h"
#include "rim_capi_test_helpers.h"

#include <stdint.h>

/* --- 読み出しヘルパ --- */

static double PushTemperatureAs(double v, int unit)
{
    rim_environment_capability_t env;

    if (unit < 0) {
        RIM_EXPECT(RimPushAndWaitCurrentNoContext(
            RIM_ID_TEMPERATURE_SENSOR_A, v, RIM_CAP_ENVIRONMENT, &env, sizeof env));
    } else {
        RIM_EXPECT(RimPushAndWaitCurrentWithUnit(
            RIM_ID_TEMPERATURE_SENSOR_A, v, (rim_source_unit_t)unit,
            RIM_CAP_ENVIRONMENT, &env, sizeof env));
    }

    return env.temperature;
}

static double PushTemperature(double v) { return PushTemperatureAs(v, -1); }

static int32_t PushJobId(double v)
{
    rim_job_capability_t job;
    RIM_EXPECT(RimPushAndWaitCurrentNoContext(RIM_ID_JOB_ID, v, RIM_CAP_JOB, &job, sizeof job));
    return job.jobId;
}

static int32_t PushStapleLevel(double v)
{
    rim_consumable_capability_t cap;
    RIM_EXPECT(RimPushAndWaitCurrentNoContext(
        RIM_ID_STAPLE_LEVEL, v, RIM_CAP_CONSUMABLE, &cap, sizeof cap));
    return cap.stapleLevel;
}

static int PushJobActive(double v)
{
    rim_job_capability_t job;
    RIM_EXPECT(RimPushAndWaitCurrentNoContext(
        RIM_ID_JOB_ACTIVE, v, RIM_CAP_JOB, &job, sizeof job));
    return job.jobActive ? 1 : 0;
}

/* ============================================================
 * ① int32 の全域が double 経由でも正確であること(本件の主眼)
 * ============================================================ */
RIM_TEST(ValueInt32IsExactThroughDouble)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    /* 境界・および float なら壊れる値を含む代表値。すべて完全一致すること。 */
    static const int32_t kCases[] = {
        0, 1, -1, 2,
        255, 256,
        65535, 65536,
        16777215,           /* 2^24-1 */
        16777216,           /* 2^24   : float では表現できない値 */
        16777217,           /* 2^24+1 */
        -16777217,
        2147483647,         /* INT32_MAX */
        -2147483647,        /* -(INT32_MAX) : INT32_MIN は §6 の飽和値と紛らわしいので避ける */
    };
    for (unsigned i = 0; i < sizeof kCases / sizeof kCases[0]; ++i) {
        RIM_EXPECT_EQ_I(PushJobId((double)kCases[i]), kCases[i]);
    }

    RIManager_Stop();
    RIManager_Destroy();
}

/* ============================================================
 * ② 温度が double のまま正確に保持されること
 * ============================================================ */
RIM_TEST(ValueKelvinExactForRepresentableInputs)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    /* context なし = 既に正規化済み(ケルビン)としてそのまま採用される。 */
    RIM_EXPECT_NEAR(PushTemperature(298.15), 298.15, 1e-9);
    RIM_EXPECT_NEAR(PushTemperature(0.0),      0.0,   1e-9);
    RIM_EXPECT_NEAR(PushTemperature(273.15), 273.15, 1e-9);
    RIM_EXPECT_NEAR(PushTemperature(373.15), 373.15, 1e-9);

    RIManager_Stop();
    RIManager_Destroy();
}

/* C から単位を指定して、摂氏/華氏がケルビンへ正規化されること(本件の主眼)。 */
RIM_TEST(ValueTemperatureUnitIsNormalizedFromC)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    RIM_EXPECT_NEAR(PushTemperatureAs(0.0,   RIM_UNIT_CELSIUS),    273.15, 1e-9);  /* 0degC   = 273.15K */
    RIM_EXPECT_NEAR(PushTemperatureAs(25.0,  RIM_UNIT_CELSIUS),    298.15, 1e-9);  /* 25degC  = 298.15K */
    RIM_EXPECT_NEAR(PushTemperatureAs(32.0,  RIM_UNIT_FAHRENHEIT), 273.15, 1e-9);  /* 32degF = 0degC */
    RIM_EXPECT_NEAR(PushTemperatureAs(212.0, RIM_UNIT_FAHRENHEIT), 373.15, 1e-9);  /* 212degF = 100degC */

    /* 同じ温度は単位が違っても正規化後に一致する。 */
    RIM_EXPECT_NEAR(
        PushTemperatureAs(-40.0, RIM_UNIT_CELSIUS),
        PushTemperatureAs(-40.0, RIM_UNIT_FAHRENHEIT),
        1e-9);

    /* 明示的な kNormalized は換算しない。 */
    RIM_EXPECT_NEAR(PushTemperatureAs(298.15, RIM_UNIT_NORMALIZED), 298.15, 1e-9);

    /* 温度は double のまま保持されるため、旧実装(x100固定小数)にあった
     * 四捨五入/切り捨ての違いは今は存在しない。華氏 100degF はちょうど
     * 310.927777...K になる。 */
    RIM_EXPECT_NEAR(PushTemperatureAs(100.0, RIM_UNIT_FAHRENHEIT), 310.9277777778, 1e-9);

    RIManager_Stop();
    RIManager_Destroy();
}

/* ============================================================
 * ③ 値落ちの実体: double ではなく static_cast の 0方向切り捨て
 * ============================================================ */
RIM_TEST(ValueTruncatesTowardZeroBeyondResolution)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    /* Percent 系は整数の分解能を超える桁を 0方向へ切り捨てる。 */
    RIM_EXPECT_EQ_I(PushStapleLevel(50.9), 50);
    RIM_EXPECT_EQ_I(PushStapleLevel(99.99), 99);

    RIManager_Stop();
    RIManager_Destroy();
}

/* ============================================================
 * ④ Percent の clamp が効くこと
 * ============================================================ */
RIM_TEST(ValuePercentIsClamped)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    RIM_EXPECT_EQ_I(PushStapleLevel(0.0),     0);
    RIM_EXPECT_EQ_I(PushStapleLevel(100.0), 100);
    RIM_EXPECT_EQ_I(PushStapleLevel(-5.0),    0);   /* 下限クランプ */
    RIM_EXPECT_EQ_I(PushStapleLevel(150.0), 100);   /* 上限クランプ */

    RIManager_Stop();
    RIManager_Destroy();
}

/* ============================================================
 * ⑤ Bool は「非0 かどうか」で判定されること
 * ============================================================ */
RIM_TEST(ValueBoolIsNonZeroTest)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    RIM_EXPECT_EQ_I(PushJobActive(0.0), 0);
    RIM_EXPECT_EQ_I(PushJobActive(1.0), 1);
    RIM_EXPECT_EQ_I(PushJobActive(0.5), 1);   /* 0でなければ true */
    RIM_EXPECT_EQ_I(PushJobActive(-1.0), 1);

    RIManager_Stop();
    RIManager_Destroy();
}

/* ============================================================
 * ⑥ 既知の穴: 型の範囲外入力が検証されていない
 *
 * double->整数の範囲外変換は C/C++ 規格上 UB(未定義動作)であり、CountRule
 * (kJobId)は範囲チェックをしていない。ここでは「現在の実測挙動」を記録して
 * 可視化する。移植先のコンパイラ/最適化で結果が変わり得るため、この挙動に
 * 依存してはならない。恒久対策は Rule 側での範囲チェック追加(要別途対応)。
 * ============================================================ */
RIM_TEST(ValueOutOfRangeIsUncheckedKnownGap)
{
    RIM_EXPECT_EQ_I(RIManager_Create(), RI_SUCCESS);
    RIM_EXPECT_EQ_I(RIManager_Start(), RI_SUCCESS);

    /* 本環境(x86-64/GCC)では cvttsd2si の「不正な整数」表現である
     * INT32_MIN (-2147483648) へ飽和する。範囲外なら符号すら関係ない。 */
    RIM_EXPECT_EQ_I(PushJobId(1e20), -2147483648);
    RIM_EXPECT_EQ_I(PushJobId(-1e20), -2147483648);

    /* 注意: NaN / Inf も同様に UB。Rule が弾いていないため本テストでは投入しない
     *       (環境によりクラッシュしうる)。範囲チェック実装時に併せて対応する。 */

    RIManager_Stop();
    RIManager_Destroy();
}

void RimCapiValueTests(void)
{
    RIM_RUN(ValueInt32IsExactThroughDouble);
    RIM_RUN(ValueKelvinExactForRepresentableInputs);
    RIM_RUN(ValueTemperatureUnitIsNormalizedFromC);
    RIM_RUN(ValueTruncatesTowardZeroBeyondResolution);
    RIM_RUN(ValuePercentIsClamped);
    RIM_RUN(ValueBoolIsNonZeroTest);
    RIM_RUN(ValueOutOfRangeIsUncheckedKnownGap);
}
