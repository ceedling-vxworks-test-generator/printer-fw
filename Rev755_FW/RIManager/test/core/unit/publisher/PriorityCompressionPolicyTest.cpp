#include <gtest/gtest.h>

#include <list>

#include "rim_api.h"

#include "QueuePolicy.hpp"
#include "PublisherInput.hpp"
#include "test/core/support/NotificationTestHelper.hpp"

TEST(
    PriorityCompressionPolicyTest,
    KeepLatestReplacesOldEvent)
{
    std::list<rim::PublisherInput>
        events;

    rim::PriorityCompressionPolicy
        policy;

    policy.Insert(
        events,
        {
            test::CapabilityTarget(
                RI_CAPABILITY_ENVIRONMENT),
            rim::EventPriority::Low,
            rim::CompressionPolicy::KeepLatest
        });

    policy.Insert(
        events,
        {
            test::CapabilityTarget(
                RI_CAPABILITY_ENVIRONMENT),
            rim::EventPriority::High,
            rim::CompressionPolicy::KeepLatest
        });

    ASSERT_EQ(
        1u,
        events.size());

    EXPECT_EQ(
        rim::EventPriority::High,
        events.front().priority);
}

TEST(
    PriorityCompressionPolicyTest,
    KeepOldestPreservesFirstEvent)
{
    std::list<rim::PublisherInput>
        events;

    rim::PriorityCompressionPolicy
        policy;

    policy.Insert(
        events,
        {
            test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
            rim::EventPriority::Low,
            rim::CompressionPolicy::KeepOldest
        });

    policy.Insert(
        events,
        {
            test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
            rim::EventPriority::High,
            rim::CompressionPolicy::KeepOldest
        });
        
    ASSERT_EQ(
        1u,
        events.size());

    EXPECT_EQ(
        rim::EventPriority::Low,
        events.front().priority);
}

TEST(
    PriorityCompressionPolicyTest,
    NonePolicyKeepsBothEvents)
{
    std::list<rim::PublisherInput>
        events;

    rim::PriorityCompressionPolicy
        policy;

    policy.Insert(
        events,
        {
            test::CapabilityTarget(RI_CAPABILITY_JOB),
            rim::EventPriority::Low
        });

    policy.Insert(
        events,
        {
            test::CapabilityTarget(RI_CAPABILITY_JOB),
            rim::EventPriority::High
        });

    EXPECT_EQ(
        2u,
        events.size());
}

TEST(
    PriorityCompressionPolicyTest,
    PriorityOrderIsMaintained)
{
    std::list<rim::PublisherInput>
        events;

    rim::PriorityCompressionPolicy
        policy;


    rim::PublisherInput low
    {
        test::CapabilityTarget(RI_CAPABILITY_JOB),
        rim::EventPriority::Low
    };

    rim::PublisherInput normal
    {
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::EventPriority::Normal
    };

    rim::PublisherInput high
    {
        test::CapabilityTarget(RI_CAPABILITY_CONSUMABLE),
        rim::EventPriority::High
    };

    policy.Insert(events, low);
    policy.Insert(events, high);
    policy.Insert(events, normal);

    ASSERT_EQ(
        3u,
        events.size());

    auto it =
        events.begin();

    EXPECT_EQ(
        rim::EventPriority::High,
        it->priority);

    ++it;

    EXPECT_EQ(
        rim::EventPriority::Normal,
        it->priority);

    ++it;

    EXPECT_EQ(
        rim::EventPriority::Low,
        it->priority);
}

TEST(
    PriorityCompressionPolicyTest,
    MixedCompressionPolicies)
{
    std::list<rim::PublisherInput>
        events;

    rim::PriorityCompressionPolicy
        policy;

    // KeepLatest
    policy.Insert(
        events,
        {
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
            rim::EventPriority::Low,
            rim::CompressionPolicy::KeepLatest
        });

    policy.Insert(
        events,
        {
            test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
            rim::EventPriority::High,
            rim::CompressionPolicy::KeepLatest
        });

    // KeepOldest
    policy.Insert(
        events,
        {
            test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
            rim::EventPriority::Low,
            rim::CompressionPolicy::KeepOldest
        });

    policy.Insert(
        events,
        {
            test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
            rim::EventPriority::High,
            rim::CompressionPolicy::KeepOldest
        });

    // None
    policy.Insert(
        events,
        {
            test::CapabilityTarget(RI_CAPABILITY_JOB),
            rim::EventPriority::Low,
            rim::CompressionPolicy::None
        });

    policy.Insert(
        events,
        {
            test::CapabilityTarget(RI_CAPABILITY_JOB),
            rim::EventPriority::High,
            rim::CompressionPolicy::None
        });

    EXPECT_EQ(
        4u,
        events.size());

    std::size_t environmentCount = 0;
    std::size_t printReadyCount = 0;
    std::size_t jobCount = 0;

    for (const auto& event : events)
    {
        if (event.target.id ==
            RI_CAPABILITY_ENVIRONMENT)
        {
            ++environmentCount;
        }

        if (event.target.id ==
            RI_CAPABILITY_PRINT_READY)
        {
            ++printReadyCount;
        }

        if (event.target.id ==
            RI_CAPABILITY_JOB)
        {
            ++jobCount;
        }
    }

    EXPECT_EQ(
        1u,
        environmentCount);

    EXPECT_EQ(
        1u,
        printReadyCount);

    EXPECT_EQ(
        2u,
        jobCount);
}

TEST(
    PriorityCompressionPolicyTest,
    SamePriorityPreservesInsertionOrder)
{
    std::list<rim::PublisherInput>
        events;

    rim::PriorityCompressionPolicy
        policy;

    rim::PublisherInput first
    {
        test::CapabilityTarget(RI_CAPABILITY_ENVIRONMENT),
        rim::EventPriority::Normal
    };

    rim::PublisherInput second
    {
        test::CapabilityTarget(RI_CAPABILITY_PRINT_READY),
        rim::EventPriority::Normal
    };

    policy.Insert(events, first);
    policy.Insert(events, second);

    ASSERT_EQ(
        2u,
        events.size());

    auto it = events.begin();

    EXPECT_EQ(
        RI_CAPABILITY_ENVIRONMENT,
        it->target.id);

    ++it;

    EXPECT_EQ(
        RI_CAPABILITY_PRINT_READY,
        it->target.id);
}