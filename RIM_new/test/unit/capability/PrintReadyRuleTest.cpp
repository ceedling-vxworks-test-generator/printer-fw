#include <gtest/gtest.h>

#include "capability/PrintReadyRule.hpp"

TEST(
    PrintReadyRuleTest,
    ReadyWhenNoError)
{
    rim::RIMSnapshot snapshot{};

    snapshot.upperDoorOpen = false;
    snapshot.rightDoorOpen = false;
    snapshot.leftDoorOpen = false;

    rim::PrintReadyRule rule;

    auto cap =
        rule.Evaluate(
            snapshot);

    EXPECT_TRUE(
        cap.ready);
}