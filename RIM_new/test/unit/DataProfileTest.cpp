#include <gtest/gtest.h>

#include "adapter/PrinterADataProfile.hpp"

using namespace rim;

TEST(DataProfileTest, ClassifyMapsIdToKind)
{
    PrinterADataProfile p;
    EXPECT_EQ(p.Classify(RIMDataId::kFaultCode),   InputKind::kFault);
    EXPECT_EQ(p.Classify(RIMDataId::kJobProgress), InputKind::kOperationReport);
    EXPECT_EQ(p.Classify(RIMDataId::kTemperature), InputKind::kCurrentValue);
    EXPECT_EQ(p.Classify(RIMDataId::kHumidity),    InputKind::kCurrentValue);
    EXPECT_EQ(p.Classify(RIMDataId::kPressure),    InputKind::kCurrentValue);
}

TEST(DataProfileTest, ConvertTemperatureToCelsiusX100)
{
    PrinterADataProfile p;
    DataContext ctx;
    auto v = p.Convert(RIMDataId::kTemperature, 25.5, ctx);
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->type, ValueType::kCelsiusX100);
    EXPECT_EQ(v->u.celsiusX100, 2550);
}

TEST(DataProfileTest, ConvertTemperatureAppliesScale)
{
    PrinterADataProfile p;
    DataContext ctx;
    ctx.scaleX1000 = 2000;  // ×2
    auto v = p.Convert(RIMDataId::kTemperature, 10.0, ctx);
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->u.celsiusX100, 2000);  // 10 * 2 * 100
}

TEST(DataProfileTest, ConvertHumidityClamps)
{
    PrinterADataProfile p;
    DataContext ctx;
    EXPECT_EQ(p.Convert(RIMDataId::kHumidity, 55.0, ctx)->u.percent, 55);
    EXPECT_EQ(p.Convert(RIMDataId::kHumidity, 150.0, ctx)->u.percent, 100);
    EXPECT_EQ(p.Convert(RIMDataId::kHumidity, -5.0, ctx)->u.percent, 0);
}

TEST(DataProfileTest, ConvertFaultCode)
{
    PrinterADataProfile p;
    DataContext ctx;
    auto v = p.Convert(RIMDataId::kFaultCode, 0x1001, ctx);
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->type, ValueType::kFaultCode);
    EXPECT_EQ(v->u.faultCode, 0x1001u);
}
