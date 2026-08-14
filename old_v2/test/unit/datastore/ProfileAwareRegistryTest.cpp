#include <gtest/gtest.h>

#include "products/printer_a/DataItem/PrinterADataDefinitionProvider.hpp"
#include "ProfileAwareDataDefinitionRegistry.hpp"

TEST(
    ProfileAwareRegistryTest,
    FindTemperature)
{
    rim::PrinterADataDefinitionProvider provider;

    rim::ProfileAwareDataDefinitionRegistry registry(
        provider);

    const auto* def =
        registry.Find(
            rim::RIMDataId::kTemperatureSensorA);

    ASSERT_NE(
        def,
        nullptr);

    EXPECT_EQ(
        def->domain,
        rim::DataDomain::kDevice);

    EXPECT_EQ(
        def->valueType,
        rim::ValueType::kDouble);
}

TEST(
    ProfileAwareRegistryTest,
    FindJobActive)
{
    rim::PrinterADataDefinitionProvider provider;

    rim::ProfileAwareDataDefinitionRegistry registry(
        provider);

    const auto* def =
        registry.Find(
            rim::RIMDataId::kJobActive);

    ASSERT_NE(
        def,
        nullptr);

    EXPECT_EQ(
        def->domain,
        rim::DataDomain::kJob);

    EXPECT_EQ(
        def->valueType,
        rim::ValueType::kBool);
}

TEST(
    ProfileAwareRegistryTest,
    UnknownIdReturnsNullptr)
{
    rim::PrinterADataDefinitionProvider provider;

    rim::ProfileAwareDataDefinitionRegistry registry(
        provider);

    const auto* def =
        registry.Find(
            static_cast<rim::RIMDataId>(
                9999));

    EXPECT_EQ(
        def,
        nullptr);
}