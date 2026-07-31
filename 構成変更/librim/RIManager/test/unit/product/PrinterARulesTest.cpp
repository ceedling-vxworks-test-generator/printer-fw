#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>

#include "RIMValueAccessor.hpp"

#include "DataItem/PrinterARules.hpp"

//
// 正規化規則。要点は **同じ id へ摂氏でも華氏でも投入できる** こと。
// どちらで来たかは context.unit で判別し、後段へはケルビンで渡す
// (単位ごとに id を増やさずに済ませるための仕組み)。
//

namespace
{

double ConvertTemperature(
    double value,
    const rim::DataContext& ctx)
{
    const rim::TemperatureRule rule;

    const auto converted =
        rule.Convert(
            rim::RawValue::Scalar(value),
            ctx);

    if (!converted)
    {
        return NAN;
    }

    double kelvin = 0.0;

    if (!rim::RIMValueAccessor::GetDouble(
            *converted,
            kelvin))
    {
        return NAN;
    }

    return kelvin;
}

rim::DataContext WithUnit(
    rim::SourceUnit unit)
{
    rim::DataContext ctx{};

    ctx.unit = unit;

    return ctx;
}

}

TEST(
    PrinterARulesTest,
    CelsiusIsConvertedToKelvin)
{
    EXPECT_NEAR(
        ConvertTemperature(
            0.0,
            WithUnit(rim::SourceUnit::kCelsius)),
        273.15,
        1e-9);

    EXPECT_NEAR(
        ConvertTemperature(
            25.0,
            WithUnit(rim::SourceUnit::kCelsius)),
        298.15,
        1e-9);
}

TEST(
    PrinterARulesTest,
    FahrenheitIsConvertedToKelvin)
{
    // 32degF = 0degC = 273.15K
    EXPECT_NEAR(
        ConvertTemperature(
            32.0,
            WithUnit(rim::SourceUnit::kFahrenheit)),
        273.15,
        1e-9);

    // 212degF = 100degC = 373.15K
    EXPECT_NEAR(
        ConvertTemperature(
            212.0,
            WithUnit(rim::SourceUnit::kFahrenheit)),
        373.15,
        1e-9);
}

TEST(
    PrinterARulesTest,
    CelsiusAndFahrenheitAgreeForTheSameTemperature)
{
    // 同じ温度を別々の単位で入れたら、正規化後は一致すること。
    // これが崩れると「送り方によって状態が変わる」ことになる。
    for (int c = -40; c <= 200; ++c)
    {
        const double celsius = static_cast<double>(c);
        const double fahrenheit = celsius * 9.0 / 5.0 + 32.0;

        const double fromC =
            ConvertTemperature(
                celsius,
                WithUnit(rim::SourceUnit::kCelsius));

        const double fromF =
            ConvertTemperature(
                fahrenheit,
                WithUnit(rim::SourceUnit::kFahrenheit));

        ASSERT_NEAR(fromC, fromF, 1e-9);
    }
}

TEST(
    PrinterARulesTest,
    NormalizedUnitIsPassedThrough)
{
    // 既に正規化済みとして扱う(勝手に換算しない)
    EXPECT_NEAR(
        ConvertTemperature(
            300.0,
            WithUnit(rim::SourceUnit::kNormalized)),
        300.0,
        1e-9);
}

TEST(
    PrinterARulesTest,
    MissingUnitIsTreatedAsNormalized)
{
    // context 未指定でも落ちず、換算もしない(単位不明を推測しない)
    const rim::DataContext empty{};

    EXPECT_NEAR(
        ConvertTemperature(
            300.0,
            empty),
        300.0,
        1e-9);
}

TEST(
    PrinterARulesTest,
    ScaleIsApplied)
{
    rim::DataContext ctx =
        WithUnit(rim::SourceUnit::kCelsius);

    // 生値が 1/10 のスケールで来る場合(250 -> 25.0degC)
    ctx.scaleX1000 = 100;

    EXPECT_NEAR(
        ConvertTemperature(
            250.0,
            ctx),
        298.15,
        1e-9);
}

TEST(
    PrinterARulesTest,
    NonScalarInputIsRejected)
{
    const rim::TemperatureRule rule;

    const std::int32_t values[2] = {1, 2};

    // スカラ以外は変換できない(黙って0にしない)
    EXPECT_FALSE(
        rule.Convert(
            rim::RawValue::Array(values, 2),
            rim::DataContext{})
            .has_value());
}

TEST(
    PrinterARulesTest,
    PercentIsClamped)
{
    const rim::PercentRule rule;

    std::int32_t value = 0;

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetInt32(
            *rule.Convert(
                rim::RawValue::Scalar(-10.0),
                rim::DataContext{}),
            value));

    EXPECT_EQ(value, 0);

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetInt32(
            *rule.Convert(
                rim::RawValue::Scalar(150.0),
                rim::DataContext{}),
            value));

    EXPECT_EQ(value, 100);

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetInt32(
            *rule.Convert(
                rim::RawValue::Scalar(50.0),
                rim::DataContext{}),
            value));

    EXPECT_EQ(value, 50);
}

TEST(
    PrinterARulesTest,
    BoolRuleTreatsNonZeroAsTrue)
{
    const rim::BoolRule rule;

    bool value = false;

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetBool(
            *rule.Convert(
                rim::RawValue::Scalar(1.0),
                rim::DataContext{}),
            value));

    EXPECT_TRUE(value);

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetBool(
            *rule.Convert(
                rim::RawValue::Scalar(0.0),
                rim::DataContext{}),
            value));

    EXPECT_FALSE(value);
}
