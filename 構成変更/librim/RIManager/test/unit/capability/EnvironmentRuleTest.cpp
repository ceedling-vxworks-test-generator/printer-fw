#include <gtest/gtest.h>

#include "EnvironmentRule.hpp"

#include "RIMValueFactory.hpp"

TEST(
    EnvironmentRuleTest,
    BuildEnvironmentCapability)
{
    rim::RIMSnapshot snapshot{};

    {
        rim::RIMDataItem item{};

        item.id =
            rim::RIMDataId::kTemperatureSensorA;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                300.15);

        snapshot.items.push_back(
            item);
    }

    {
        rim::RIMDataItem item{};

        item.id =
            rim::RIMDataId::kHumiditySensor;

        item.valueType =
            rim::ValueType::kDouble;

        item.value =
            rim::RIMValueFactory::CreateDouble(
                60.0);

        snapshot.items.push_back(
            item);
    }

    rim::EnvironmentRule rule;

    auto cap =
        rule.Evaluate(
            snapshot);

    EXPECT_DOUBLE_EQ(
        cap.temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        cap.humidity,
        60.0);
}