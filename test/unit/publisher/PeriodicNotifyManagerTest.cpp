#include <gtest/gtest.h>

#include "PeriodicNotifyManager.hpp"

TEST(
    PeriodicNotifyManagerTest,
    GetDueConditions)
{
    rim::PeriodicNotifyManager manager;

    rim::PeriodicCondition condition{};

    condition.subscriptionId = 100;

    condition.interval =
        std::chrono::seconds(
            1);

    condition.nextNotifyTime =
        std::chrono::steady_clock::now()
        - std::chrono::seconds(
            1);

    manager.Register(
        condition);

    std::vector<
        rim::PeriodicCondition>
        due;

    manager.GetDueConditions(
        due);

    ASSERT_EQ(
        due.size(),
        1U);
}

TEST(
    PeriodicNotifyManagerTest,
    UpdateNextTime)
{
    rim::PeriodicNotifyManager manager;

    rim::PeriodicCondition condition{};

    condition.subscriptionId = 100;

    condition.interval =
        std::chrono::seconds(
            10);

    condition.nextNotifyTime =
        std::chrono::steady_clock::now()
        - std::chrono::seconds(
            1);

    manager.Register(
        condition);

    manager.UpdateNextTime(
        100);

    std::vector<
        rim::PeriodicCondition>
        due;

    manager.GetDueConditions(
        due);

    EXPECT_TRUE(
        due.empty());
}
TEST(
    PeriodicNotifyManagerTest,
    GetOnlyDueConditions)
{
    rim::PeriodicNotifyManager manager;

    rim::PeriodicCondition dueCondition{};

    dueCondition.subscriptionId = 100;
    dueCondition.interval =
        std::chrono::seconds(
            1);

    dueCondition.nextNotifyTime =
        std::chrono::steady_clock::now()
        - std::chrono::seconds(
            1);

    rim::PeriodicCondition futureCondition{};

    futureCondition.subscriptionId = 200;
    futureCondition.interval =
        std::chrono::seconds(
            1);

    futureCondition.nextNotifyTime =
        std::chrono::steady_clock::now()
        + std::chrono::seconds(
            10);

    manager.Register(
        dueCondition);

    manager.Register(
        futureCondition);

    std::vector<
        rim::PeriodicCondition>
        due;

    manager.GetDueConditions(
        due);

    ASSERT_EQ(
        due.size(),
        1U);

    EXPECT_EQ(
        due.front().subscriptionId,
        100U);
}
