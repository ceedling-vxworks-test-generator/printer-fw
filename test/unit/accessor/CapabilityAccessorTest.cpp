#include <gtest/gtest.h>

#include <any>

#include "CapabilityAccessor.hpp"
#include "CapabilityStore.hpp"

#include "EnvironmentCapability.hpp"

#include "rim_capability_id.h"

TEST(
    CapabilityAccessorTest,
    GetEnvironment)
{
    rim::CapabilityStore store;

    rim::EnvironmentCapability env{};

    env.temperature = 35.0;
    env.humidity = 60.0;

    store.Store(
        RI_CAPABILITY_ENVIRONMENT,
        env);

    rim::CapabilityAccessor accessor(
        store);

    const auto result =
        accessor.Get<
            rim::EnvironmentCapability>(
                RI_CAPABILITY_ENVIRONMENT);

    EXPECT_DOUBLE_EQ(
        result.temperature,
        35.0);

    EXPECT_DOUBLE_EQ(
        result.humidity,
        60.0);
}

// TEST(
//     CapabilityAccessorTest,
//     GetJob)
// {
//     rim::CapabilityStore store;

//     rim::JobCapability capability{};

//     capability.jobActive = true;
//     capability.jobId = 123;

//     store.Store(
//         RI_CAPABILITY_JOB,
//         capability);

//     rim::CapabilityAccessor accessor(
//         store);

//     const auto result =
//         accessor.Get<
//             rim::JobCapability>(
//                 RI_CAPABILITY_JOB);

//     EXPECT_TRUE(
//         result.jobActive);

//     EXPECT_EQ(
//         result.jobId,
//         123);
// }

TEST(
    CapabilityAccessorTest,
    ThrowWhenCapabilityNotFound)
{
    rim::CapabilityStore store;

    rim::CapabilityAccessor accessor(
        store);

    EXPECT_THROW(
        (
            accessor.Get<
                rim::EnvironmentCapability>(
                    RI_CAPABILITY_ENVIRONMENT)
        ),
        std::exception);
}