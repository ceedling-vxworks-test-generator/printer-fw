#include <gtest/gtest.h>

#include "adapter/PrinterADataProfile.hpp"

using namespace rim;

namespace
{
ValueType ConvertedType(const IRule* rule, double scalar)
{
    if (!rule) return ValueType::kNone;
    DataContext ctx;
    auto v = rule->Convert(RawValue::Scalar(scalar), ctx);
    return v ? v->type : ValueType::kNone;
}
} // namespace

// SelectRule/Classify はテーブル駆動(PrinterADataProfile.cpp の kTable)。
// static_assert は行数(kRIMDataIdCount との一致)しか見ないため、行の欠落・取り違えは
// ここで検知する回帰テストが担う。

TEST(PrinterADataProfileTest, SelectRuleAndClassifyCoverAllIds)
{
    PrinterADataProfile profile;
    for (int i = 0; i < kRIMDataIdCount; ++i) {
        const auto id = static_cast<RIMDataId>(i);
        EXPECT_NE(profile.SelectRule(id), nullptr) << "id=" << i;
        EXPECT_TRUE(profile.Classify(id).has_value()) << "id=" << i;
    }
}

TEST(PrinterADataProfileTest, OutOfRangeIdReturnsEmpty)
{
    PrinterADataProfile profile;
    EXPECT_EQ(profile.SelectRule(RIMDataId::kCount), nullptr);
    EXPECT_FALSE(profile.Classify(RIMDataId::kCount).has_value());
}

TEST(PrinterADataProfileTest, SelectRuleMapsExpectedRuleKind)
{
    PrinterADataProfile profile;
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kTemperature),      25.0), ValueType::kKelvinX100);
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kHumidity),         50.0), ValueType::kPercent);
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kPressure),         50.0), ValueType::kPercent);
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kInkLevel),         50.0), ValueType::kPercent);
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kWiperLevel),       50.0), ValueType::kPercent);
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kJobProgress),      50.0), ValueType::kJobProgress);
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kFaultCode),        0x10), ValueType::kFaultCode);
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kCoverOpen),         1.0), ValueType::kBool);
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kEStop),             1.0), ValueType::kBool);
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kUnitAlive),         1.0), ValueType::kBool);
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kMaintenanceCount), 5.0),  ValueType::kUInt32);
}

TEST(PrinterADataProfileTest, ClassifyMapsExpectedKind)
{
    PrinterADataProfile profile;
    EXPECT_EQ(*profile.Classify(RIMDataId::kFaultCode),        InputKind::kFault);
    EXPECT_EQ(*profile.Classify(RIMDataId::kJobProgress),      InputKind::kOperationReport);
    EXPECT_EQ(*profile.Classify(RIMDataId::kTemperature),      InputKind::kCurrentValue);
    EXPECT_EQ(*profile.Classify(RIMDataId::kHumidity),         InputKind::kCurrentValue);
    EXPECT_EQ(*profile.Classify(RIMDataId::kPressure),         InputKind::kCurrentValue);
    EXPECT_EQ(*profile.Classify(RIMDataId::kInkLevel),         InputKind::kCurrentValue);
    EXPECT_EQ(*profile.Classify(RIMDataId::kWiperLevel),       InputKind::kCurrentValue);
    EXPECT_EQ(*profile.Classify(RIMDataId::kCoverOpen),        InputKind::kCurrentValue);
    EXPECT_EQ(*profile.Classify(RIMDataId::kEStop),            InputKind::kCurrentValue);
    EXPECT_EQ(*profile.Classify(RIMDataId::kMaintenanceCount), InputKind::kCurrentValue);
    EXPECT_EQ(*profile.Classify(RIMDataId::kUnitAlive),        InputKind::kCurrentValue);
}

// ============================================================
// TemperatureRule: context.unit による単位分岐 → 絶対温度(ケルビン)×100 へ正規化
// 同じ id(kTemperature)へ摂氏でも華氏でも投入でき、DataStore へは常にケルビンで入る。
// ============================================================
namespace
{
// 指定単位で温度を変換し、ケルビン×100 を返す(変換不能なら nullopt)。
std::optional<std::int32_t> ConvertTemp(double raw, std::optional<SourceUnit> unit)
{
    PrinterADataProfile profile;
    const IRule* rule = profile.SelectRule(RIMDataId::kTemperature);
    if (!rule) return std::nullopt;

    DataContext ctx;
    ctx.unit = unit;
    auto v = rule->Convert(RawValue::Scalar(raw), ctx);
    if (!v || v->type != ValueType::kKelvinX100) return std::nullopt;
    return v->u.kelvinX100;
}
} // namespace

TEST(TemperatureRuleTest, CelsiusIsNormalizedToKelvin)
{
    // K = C + 273.15
    EXPECT_EQ(ConvertTemp(0.0,     SourceUnit::kCelsius), 27315);   // 0℃    = 273.15K
    EXPECT_EQ(ConvertTemp(25.0,    SourceUnit::kCelsius), 29815);   // 25℃   = 298.15K
    EXPECT_EQ(ConvertTemp(100.0,   SourceUnit::kCelsius), 37315);   // 100℃  = 373.15K
    EXPECT_EQ(ConvertTemp(-273.15, SourceUnit::kCelsius), 0);       // 絶対零度
}

TEST(TemperatureRuleTest, FahrenheitIsNormalizedToKelvin)
{
    // K = (F - 32) × 5/9 + 273.15
    EXPECT_EQ(ConvertTemp(32.0,   SourceUnit::kFahrenheit), 27315); // 32°F  = 0℃    = 273.15K
    EXPECT_EQ(ConvertTemp(212.0,  SourceUnit::kFahrenheit), 37315); // 212°F = 100℃  = 373.15K
    EXPECT_EQ(ConvertTemp(-459.67, SourceUnit::kFahrenheit), 0);    // 絶対零度
}

// 摂氏と華氏で同じ温度を表す入力は、正規化後に一致すること(単位差の吸収そのもの)。
TEST(TemperatureRuleTest, CelsiusAndFahrenheitAgreeForSameTemperature)
{
    EXPECT_EQ(ConvertTemp(0.0,   SourceUnit::kCelsius), ConvertTemp(32.0,  SourceUnit::kFahrenheit));
    EXPECT_EQ(ConvertTemp(100.0, SourceUnit::kCelsius), ConvertTemp(212.0, SourceUnit::kFahrenheit));
    EXPECT_EQ(ConvertTemp(-40.0, SourceUnit::kCelsius), ConvertTemp(-40.0, SourceUnit::kFahrenheit)); // 一致点
}

// context 未指定 / kNormalized は「既に正規化済み」として換算しない(規定値の扱い)。
TEST(TemperatureRuleTest, MissingUnitIsTreatedAsAlreadyNormalized)
{
    EXPECT_EQ(ConvertTemp(298.15, std::nullopt),                29815);
    EXPECT_EQ(ConvertTemp(298.15, SourceUnit::kNormalized),     29815);
    // 摂氏として解釈されていない(されていれば 57130 になるはず)ことを明示。
    EXPECT_NE(ConvertTemp(298.15, std::nullopt), ConvertTemp(298.15, SourceUnit::kCelsius));
}

// 華氏→摂氏は割り切れないため、本Ruleは切り捨てではなく四捨五入する。
TEST(TemperatureRuleTest, FahrenheitRoundsInsteadOfTruncating)
{
    // 100°F = 37.7777…℃ = 310.9277…K → 四捨五入で 31093(切り捨てなら 31092)
    EXPECT_EQ(ConvertTemp(100.0, SourceUnit::kFahrenheit), 31093);
}

// スカラ以外(構造体/配列)は温度Ruleでは受理しない。
TEST(TemperatureRuleTest, NonScalarIsRejected)
{
    PrinterADataProfile profile;
    const IRule* rule = profile.SelectRule(RIMDataId::kTemperature);
    ASSERT_NE(rule, nullptr);

    DataContext ctx; ctx.unit = SourceUnit::kCelsius;
    struct Packet { std::uint8_t v; } pkt{25};
    EXPECT_FALSE(rule->Convert(RawValue::Struct(pkt), ctx).has_value());
}
