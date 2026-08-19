#include <gtest/gtest.h>

#include "products/printer_a/DataItem/PrinterANormalizationFunctions.hpp"

#include "RIMValueFactory.hpp"
#include "RIMValueAccessor.hpp"

TEST(
    NormalizationFunctionTest,
    TemperatureSensorA)
{
    auto input =
        rim::RIMValueFactory::CreateDouble(
            30.0);

    auto output =
        rim::NormalizeTemperatureSensorA(
            input,
            nullptr);

    double result{};

    ASSERT_TRUE(
        rim::RIMValueAccessor::GetDouble(
            output,
            result));

    EXPECT_DOUBLE_EQ(
        result,
        303.15);
}

TEST(
    NormalizationFunctionTest,
    IdentityBinary)
{
    rim::BinaryStoreValue value;

    auto input =
        rim::RIMValueFactory::CreateBinary(
            &value);

    auto result =
        rim::IdentityNormalize(
            input,
            nullptr);

    EXPECT_EQ(
        result.type,
        rim::ValueType::kBinary);
}
