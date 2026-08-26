#include <gtest/gtest.h>

#include <thread>
#include <vector>

#include "VersionedStoreState.hpp"

namespace rim
{

TEST(
    VersionedStoreStateTest,
    InitialVersionIsZero)
{
    VersionedStoreState state;

    EXPECT_EQ(
        state.Get().value,
        0u);
}

TEST(
    VersionedStoreStateTest,
    IncrementUpdatesVersion)
{
    VersionedStoreState state;

    const auto before =
        state.Get();

    state.Increment();

    const auto after =
        state.Get();

    EXPECT_GT(
        after.value,
        before.value);
}

TEST(
    VersionedStoreStateTest,
    MultipleIncrementAccumulates)
{
    VersionedStoreState state;

    state.Increment();
    state.Increment();
    state.Increment();

    EXPECT_EQ(
        state.Get().value,
        3u);
}

TEST(
    VersionedStoreStateTest,
    ConcurrentIncrement)
{
    VersionedStoreState state;

    constexpr std::size_t kThreadCount = 8;
    constexpr std::size_t kIncrementPerThread = 10000;

    std::vector<std::thread> threads;
    threads.reserve(
        kThreadCount);

    for (std::size_t i = 0;
         i < kThreadCount;
         ++i)
    {
        threads.emplace_back(
            [&state]      {
                for (std::size_t j = 0;
                     j < kIncrementPerThread;
                     ++j)
                {
                    state.Increment();
                }
            });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    EXPECT_EQ(
        state.Get().value,
        kThreadCount * kIncrementPerThread);
}

} // namespace rim