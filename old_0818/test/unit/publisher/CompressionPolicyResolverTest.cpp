#include <gtest/gtest.h>

#include "printer_a.h"

#include "CompressionPolicyResolver.hpp"

TEST(
    CompressionPolicyResolverTest,
    EnvironmentKeepLatest)
{
    EXPECT_EQ(
        rim::CompressionPolicy::KeepLatest,
        rim::CompressionPolicyResolver::
            Resolve(
            {
                rim::NotificationTargetType::
                    Capability,
                RI_CAPABILITY_ENVIRONMENT
            }));
}

TEST(
    CompressionPolicyResolverTest,
    ConsumableKeepLatest)
{
    EXPECT_EQ(
        rim::CompressionPolicy::KeepLatest,
        rim::CompressionPolicyResolver::
            Resolve(
            {
                rim::NotificationTargetType::
                    Capability,
                RI_CAPABILITY_CONSUMABLE
            }));
}

TEST(
    CompressionPolicyResolverTest,
    PrintReadyKeepOldest)
{
    EXPECT_EQ(
        rim::CompressionPolicy::KeepOldest,
        rim::CompressionPolicyResolver::
            Resolve(
            {
                rim::NotificationTargetType::
                    Capability,
                RI_CAPABILITY_PRINT_READY
            }));
}

TEST(
    CompressionPolicyResolverTest,
    JobNone)
{
    EXPECT_EQ(
        rim::CompressionPolicy::None,
        rim::CompressionPolicyResolver::
            Resolve(
            {
                rim::NotificationTargetType::
                    Capability,
                RI_CAPABILITY_JOB
            }));
}

TEST(
    CompressionPolicyResolverTest,
    UnknownReturnsNone)
{
    EXPECT_EQ(
        rim::CompressionPolicy::None,
        rim::CompressionPolicyResolver::
            Resolve(
            {
                rim::NotificationTargetType::
                    Capability,
                999999
            }));
}

TEST(
    CompressionPolicyResolverTest,
    DifferentTargetTypeReturnsNone)
{
    EXPECT_EQ(
        rim::CompressionPolicy::None,
        rim::CompressionPolicyResolver::Resolve(
        {
            rim::NotificationTargetType::Data,
            RI_CAPABILITY_ENVIRONMENT
        }));
}