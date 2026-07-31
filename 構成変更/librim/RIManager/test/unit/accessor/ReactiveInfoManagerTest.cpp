#include <gtest/gtest.h>

#include "NotificationReceiver.hpp"

TEST(
    ReactiveInfoManagerTest,
    TryGetEnvironment)
{
    rim::SubscriberMailbox mailbox;

    rim::EnvironmentCapability input{};

    input.temperature = 300.15;
    input.humidity = 60.0;

    mailbox.Push(
        input);

    rim::NotificationReceiver manager(
        mailbox);

    rim::EnvironmentCapability output{};

    ASSERT_TRUE(
        manager.TryGetEnvironment(
            output));

    EXPECT_DOUBLE_EQ(
        output.temperature,
        300.15);

    EXPECT_DOUBLE_EQ(
        output.humidity,
        60.0);
}