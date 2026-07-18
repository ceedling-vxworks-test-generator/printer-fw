#include <gtest/gtest.h>

#include "capability/EnvironmentRule.hpp"

TEST(
    EnvironmentRuleTest,
    BuildEnvironmentCapability)
{
    rim::RIMSnapshot snapshot{};

    snapshot.temperature = 300.15;
    snapshot.humidity = 60.0;

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