#pragma once

#include <gtest/gtest.h>

#include "PublisherInput.hpp"
#include "PublisherInputQueue.hpp"

namespace rim::test
{

inline PublisherInput PopNotification(
    PublisherInputQueue& queue)
{
    PublisherInput event{};

    EXPECT_TRUE(
        queue.TryPop(
            event));

    return event;
}

inline void ExpectDataNotification(
    PublisherInputQueue& queue)
{
    PublisherInput event{};

    ASSERT_TRUE(
        queue.TryPop(
            event));

    EXPECT_EQ(
        NotificationTargetType::Data,
        event.target.type);
}

inline void ExpectCapabilityNotification(
    PublisherInputQueue& queue,
    RICapabilityId capabilityId)
{
    PublisherInput event{};

    ASSERT_TRUE(
        queue.TryPop(
            event));

    EXPECT_EQ(
        NotificationTargetType::Capability,
        event.target.type);

    EXPECT_EQ(
        static_cast<std::uint32_t>(
            capabilityId),
        event.target.id);
}

inline void ExpectNoNotification(
    PublisherInputQueue& queue)
{
    PublisherInput event{};

    EXPECT_FALSE(
        queue.TryPop(
            event));
}

} // namespace rim::test