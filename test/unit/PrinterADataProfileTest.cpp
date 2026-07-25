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
    EXPECT_EQ(ConvertedType(profile.SelectRule(RIMDataId::kTemperature),      25.0), ValueType::kCelsiusX100);
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
