/*
 * rim_capi_value_test.c - capi の「値の保証」テスト。
 *
 * 受理点が double 1本(rim_raw_scalar)であるため、整数値が double を経由することによる
 * 値落ちを懸念する声があった。本ファイルはその実挙動を固定する回帰テストである。
 *
 * 結論(実測で確認済み):
 *   - double の仮数部は53bitあるため、int32/uint32 の全域は double を経由しても完全に正確。
 *     float(仮数部24bit)では失われる 2^24+1 のような値も double では保たれる。
 *   - **値落ちの原因は double ではなく Rule 内の static_cast による切り捨て**である。
 *     小数部は四捨五入されず 0 方向へ切り捨てられる(§ TruncatesTowardZero)。
 *   - 型の範囲外(負値→uint32、巨大値)は規格上 UB。現状 Rule が弾いていないため、
 *     実測値を記録して既知の穴として可視化する(§ OutOfRangeIsUnchecked)。
 */

#include "rim_capi_test.h"
#include "rim_capi.h"

#include <stdint.h>

/* --- 読み出しヘルパ(いずれも CurrentValue レーン=最新値上書きのidを使う) --- */

static int32_t PushTemperature(double v)
{
    RIM_EXPECT_EQ_I(rim_push(RIM_ID_TEMPERATURE, rim_raw_scalar(v), NULL), RIM_OK);
    rim_dispatch();
    rim_printer_status_t st = rim_get_status(RIM_DOMAIN_ENVIRONMENT, false);
    RIM_EXPECT(st.has_data && st.data.has_temperature_x100);
    return st.data.temperature_x100;
}

static uint32_t PushMaintCount(double v)
{
    RIM_EXPECT_EQ_I(rim_push(RIM_ID_MAINTENANCE_COUNT, rim_raw_scalar(v), NULL), RIM_OK);
    rim_dispatch();
    rim_printer_status_t st = rim_get_status(RIM_DOMAIN_MAINTENANCE, false);
    RIM_EXPECT(st.has_data && st.data.has_maint_counter);
    return st.data.maint_counter;
}

static uint8_t PushInkLevel(double v)
{
    RIM_EXPECT_EQ_I(rim_push(RIM_ID_INK_LEVEL, rim_raw_scalar(v), NULL), RIM_OK);
    rim_dispatch();
    rim_printer_status_t st = rim_get_status(RIM_DOMAIN_CONSUMABLE, false);
    RIM_EXPECT(st.has_data && st.data.has_ink_level);
    return st.data.ink_level;
}

static int PushUnitAlive(double v)
{
    RIM_EXPECT_EQ_I(rim_push(RIM_ID_UNIT_ALIVE, rim_raw_scalar(v), NULL), RIM_OK);
    rim_dispatch();
    rim_printer_status_t st = rim_get_status(RIM_DOMAIN_HEALTH, false);
    RIM_EXPECT(st.has_data && st.data.has_unit_alive);
    return st.data.unit_alive ? 1 : 0;
}

/* ============================================================
 * ① uint32 の全域が double 経由でも正確であること(本件の主眼)
 * ============================================================ */
RIM_TEST(ValueUInt32IsExactThroughDouble)
{
    /* 境界・および float なら壊れる値を含む代表値。すべて完全一致すること。 */
    static const uint32_t kCases[] = {
        0u, 1u, 2u,
        255u, 256u,
        65535u, 65536u,
        16777215u,          /* 2^24-1 */
        16777216u,          /* 2^24   */
        16777217u,          /* 2^24+1 : float では表現できない値 */
        2147483647u,        /* INT32_MAX */
        2147483648u,        /* INT32_MAX+1 */
        4294967294u,        /* UINT32_MAX-1 */
        4294967295u         /* UINT32_MAX */
    };
    for (unsigned i = 0; i < sizeof kCases / sizeof kCases[0]; ++i) {
        const uint32_t in = kCases[i];
        RIM_EXPECT_EQ_U(PushMaintCount((double)in), in);
    }
}

/* uint32 の全ビット位置(1ビットずつ立てた値)でも往復が壊れないこと。 */
RIM_TEST(ValueUInt32AllBitPositionsRoundTrip)
{
    for (int bit = 0; bit < 32; ++bit) {
        const uint32_t in = (uint32_t)1u << bit;
        RIM_EXPECT_EQ_U(PushMaintCount((double)in), in);
    }
}

/* ============================================================
 * ② 整数値の温度(x100固定小数)が正確であること
 * ============================================================ */
RIM_TEST(ValueCelsiusExactForRepresentableInputs)
{
    RIM_EXPECT_EQ_I(PushTemperature(25.0),   2500);
    RIM_EXPECT_EQ_I(PushTemperature(0.0),       0);
    RIM_EXPECT_EQ_I(PushTemperature(-40.0), -4000);
    RIM_EXPECT_EQ_I(PushTemperature(25.6),   2560);   /* 0.01刻みで表せる小数 */
    RIM_EXPECT_EQ_I(PushTemperature(-2.5),   -250);
    RIM_EXPECT_EQ_I(PushTemperature(100.25), 10025);
}

/* ============================================================
 * ③ 値落ちの実体: double ではなく static_cast の 0方向切り捨て
 * ============================================================ */
RIM_TEST(ValueTruncatesTowardZeroBeyondResolution)
{
    /* x100 の分解能(0.01)より細かい桁は、四捨五入ではなく切り捨てられる。 */
    RIM_EXPECT_EQ_I(PushTemperature(25.678), 2567);   /* 2568 ではない */
    RIM_EXPECT_EQ_I(PushTemperature(2.675),   267);   /* 267.5 → 267 */
    RIM_EXPECT_EQ_I(PushTemperature(1.005),   100);   /* 100.5 → 100 */

    /* 負値も 0 方向へ切り捨てる(-1 方向ではない)。 */
    RIM_EXPECT_EQ_I(PushTemperature(-0.005),    0);   /* -0.5 → 0 */
    RIM_EXPECT_EQ_I(PushTemperature(-25.678), -2567); /* -2568 ではない */

    /* Percent 系も同様に切り捨て。 */
    RIM_EXPECT_EQ_U(PushInkLevel(50.9), 50u);
    RIM_EXPECT_EQ_U(PushInkLevel(99.99), 99u);
}

/* ============================================================
 * ④ Percent の clamp が効くこと
 * ============================================================ */
RIM_TEST(ValuePercentIsClamped)
{
    RIM_EXPECT_EQ_U(PushInkLevel(0.0),     0u);
    RIM_EXPECT_EQ_U(PushInkLevel(100.0), 100u);
    RIM_EXPECT_EQ_U(PushInkLevel(-5.0),    0u);   /* 下限クランプ */
    RIM_EXPECT_EQ_U(PushInkLevel(150.0), 100u);   /* 上限クランプ */
}

/* ============================================================
 * ⑤ Bool は「非0 かどうか」で判定されること
 * ============================================================ */
RIM_TEST(ValueBoolIsNonZeroTest)
{
    RIM_EXPECT_EQ_I(PushUnitAlive(0.0), 0);
    RIM_EXPECT_EQ_I(PushUnitAlive(1.0), 1);
    RIM_EXPECT_EQ_I(PushUnitAlive(0.5), 1);   /* 0でなければ true */
    RIM_EXPECT_EQ_I(PushUnitAlive(-1.0), 1);
}

/* ============================================================
 * ⑥ 既知の穴: 型の範囲外入力が検証されていない
 *
 * double→整数の範囲外変換は C/C++ 規格上 UB(未定義動作)であり、現状どの Rule も
 * 範囲チェックをしていない。ここでは「現在の実測挙動」を記録して可視化する。
 * 移植先のコンパイラ/最適化で結果が変わり得るため、この挙動に依存してはならない。
 * 恒久対策は Rule 側での範囲チェック追加(要別途対応)。
 * ============================================================ */
RIM_TEST(ValueOutOfRangeIsUncheckedKnownGap)
{
    /* 負値 → uint32: 現状はラップアラウンドした値が入る(規格上はUB)。 */
    RIM_EXPECT_EQ_U(PushMaintCount(-1.0), 4294967295u);

    /* uint32 上限を大きく超える値: 現状は 0 になる(規格上はUB)。 */
    RIM_EXPECT_EQ_U(PushMaintCount(1e20), 0u);

    /* 注意: NaN / Inf も同様に UB。Rule が弾いていないため本テストでは投入しない
     *       (環境によりクラッシュしうる)。範囲チェック実装時に併せて対応する。 */
}

/* ============================================================
 * ⑦ 構造体・配列経路(kBytes)の戻り値契約
 * ============================================================ */
RIM_TEST(ValueStructPathRejectedByScalarRule)
{
    /* PrinterA の Rule はすべてスカラ専用。構造体を渡すと変換拒否される。 */
    struct { unsigned char level; } pkt = { 77 };
    RIM_EXPECT_EQ_I(rim_push(RIM_ID_TEMPERATURE, rim_raw_struct(&pkt, sizeof pkt), NULL),
                    RIM_ERR_CONVERT);

    unsigned char cells[3] = { 40, 50, 60 };
    RIM_EXPECT_EQ_I(rim_push(RIM_ID_TEMPERATURE, rim_raw_array(cells, sizeof cells[0], 3), NULL),
                    RIM_ERR_CONVERT);
}

void RimCapiValueTests(void)
{
    RIM_RUN(ValueUInt32IsExactThroughDouble);
    RIM_RUN(ValueUInt32AllBitPositionsRoundTrip);
    RIM_RUN(ValueCelsiusExactForRepresentableInputs);
    RIM_RUN(ValueTruncatesTowardZeroBeyondResolution);
    RIM_RUN(ValuePercentIsClamped);
    RIM_RUN(ValueBoolIsNonZeroTest);
    RIM_RUN(ValueOutOfRangeIsUncheckedKnownGap);
    RIM_RUN(ValueStructPathRejectedByScalarRule);
}
