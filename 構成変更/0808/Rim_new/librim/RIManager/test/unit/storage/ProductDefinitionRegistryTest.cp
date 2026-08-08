#include <gtest/gtest.h>

TEST(
    ProductDefinitionRegistryTest,
    FindTemperature)
{
    rim::ProductDefinitionRegistry registry(
        rim::kPrinterAProductDefinition);

    const auto* item =
        registry.Find(
            RI_DATA_TEMPERATURE_SENSOR_A);

    ASSERT_NE(item, nullptr);

    EXPECT_EQ(
        item->name,
        "TemperatureSensorA");
}

TEST(
    ProductDefinitionRegistryTest,
    FindByName)
{
    rim::ProductDefinitionRegistry registry(
        rim::kPrinterAProductDefinition);

    const auto* item =
        registry.FindByName(
            "JobId");

    ASSERT_NE(item, nullptr);

    EXPECT_EQ(
        item->id,
        rim::RIMDataId::kJobId);
}

TEST(
    ProductDefinitionRegistryTest,
    UnknownIdReturnsNullptr)
{
    rim::ProductDefinitionRegistry registry(
        rim::kPrinterAProductDefinition);

    const auto* item =
        registry.Find(
            static_cast<RIDataId>(9999));

    EXPECT_EQ(
        item,
        nullptr);
}

TEST(
    ProductDefinitionRegistryTest,
    UnknownNameReturnsNullptr)
{
    rim::ProductDefinitionRegistry registry(
        rim::kPrinterAProductDefinition);

    EXPECT_EQ(
        registry.FindByName(
            "Unknown"),
        nullptr);
}

