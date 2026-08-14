#include <gtest/gtest.h>

#include <array>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "CapabilityStorage.hpp"
#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"
#include "SnapshotStorage.hpp"
#include "ValueStore.hpp"

namespace
{

std::size_t GetVmRSSKb()
{
    std::ifstream file("/proc/self/status");

    std::string line;

    while (std::getline(file, line))
    {
        if (line.rfind("VmRSS:", 0) == 0)
        {
            std::stringstream ss(line);

            std::string key;
            std::size_t value{};

            ss >> key >> value;

            return value;
        }
    }

    return 0;
}

struct LargeCapability
{
    std::array<std::uint8_t, 1024> data{};
};

} // namespace

TEST(
    StorageMemoryTest,
    PrintMemoryReport)
{
    std::cout
        << "\n=== Storage Memory Report ===\n";

    std::cout
        << "ValueStore        : "
        << sizeof(rim::ValueStore)
        << '\n';

    std::cout
        << "CapabilityStorage : "
        << sizeof(rim::CapabilityStorage)
        << '\n';

    std::cout
        << "RIMSnapshot       : "
        << sizeof(rim::RIMSnapshot)
        << '\n';

    std::cout
        << "SnapshotStorage   : "
        << sizeof(rim::SnapshotStorage)
        << '\n';

    std::cout
        << "SnapshotEntry     : "
        << rim::SnapshotStorage::EntrySize()
        << '\n';

    std::cout
        << "SnapshotCapacity  : "
        << rim::SnapshotStorage::MaxSnapshots()
        << '\n';
}

TEST(
    StorageMemoryTest,
    ValueStoreMemoryGrowth)
{
    rim::ValueStore store;

    const auto rssStart =
        GetVmRSSKb();

    constexpr std::uint32_t kSteps[] =
    {
        100,
        1000,
        10000,
        50000,
        100000,
        1000000,
        2000000
    };

    std::uint32_t currentCount = 0;

    std::cout
        << "\n=== ValueStore Memory Growth ===\n";

    for (const auto targetCount : kSteps)
    {
        for (; currentCount < targetCount; ++currentCount)
        {
            rim::RIMDataItem item{};

            item.id =
                static_cast<RIDataId>(
                    currentCount + 1);

            item.valueType =
                rim::ValueType::kInt32;

            item.value =
                rim::RIMValueFactory::CreateInt32(
                    static_cast<std::int32_t>(
                        currentCount));

            store.Store(item);
        }

        const auto rss =
            GetVmRSSKb();

        const auto itemCount =
            store.GetAll().size();

        EXPECT_EQ(
            itemCount,
            targetCount);

        const auto deltaKb =
            rss - rssStart;

        const double bytesPerItem =
            itemCount > 0
                ? static_cast<double>(
                    deltaKb * 1024ULL) /
                    static_cast<double>(
                        itemCount)
                : 0.0;

        std::cout
            << "Stored="
            << std::setw(8)
            << itemCount

            << " RSS="
            << std::setw(8)
            << rss

            << " KB Delta="
            << std::setw(8)
            << deltaKb

            << " KB PerItem="
            << std::setw(10)
            << std::fixed
            << std::setprecision(2)
            << bytesPerItem

            << " bytes\n";
    }
}

TEST(
    StorageMemoryTest,
    CapabilityStorageMemoryGrowth)
{
    rim::CapabilityStorage storage;

    const auto rssStart =
        GetVmRSSKb();

    constexpr std::uint32_t kSteps[] =
    {
        100,
        1000,
        10000,
        50000
    };

    std::size_t previousCount = 0;
    std::size_t previousRss = rssStart;

    std::cout
        << "\n=== CapabilityStorage Memory Growth ===\n";

    for (const auto targetCount : kSteps)
    {
        for (std::uint32_t i =
                static_cast<std::uint32_t>(
                    previousCount);
             i < targetCount;
             ++i)
        {
            storage.Store(
                static_cast<RICapabilityId>(
                    i + 1),
                static_cast<std::int32_t>(
                    i));
        }

        EXPECT_EQ(
            storage.Size(),
            targetCount);

        const auto rss =
            GetVmRSSKb();

        const auto deltaKb =
            rss - rssStart;

        const auto incrementItems =
            targetCount - previousCount;

        const auto incrementKb =
            rss - previousRss;

        const double totalBytesPerItem =
            targetCount > 0
                ? static_cast<double>(
                    deltaKb * 1024ULL) /
                    static_cast<double>(
                        targetCount)
                : 0.0;

        const double incrementBytesPerItem =
            incrementItems > 0
                ? static_cast<double>(
                    incrementKb * 1024ULL) /
                    static_cast<double>(
                        incrementItems)
                : 0.0;

std::cout
    << "Stored="
    << std::setw(8)
    << storage.Size()

    << " Buckets="
    << std::setw(8)
    << storage.BucketCount()

    << " Load="
    << std::setw(6)
    << std::fixed
    << std::setprecision(2)
    << storage.LoadFactor()

    << " RSS="
    << std::setw(8)
    << rss

    << " KB Delta="
    << std::setw(8)
    << deltaKb

    << " KB PerItem="
    << std::setw(10)
    << totalBytesPerItem

    << " IncPerItem="
    << std::setw(10)
    << incrementBytesPerItem

    << " bytes\n";

        previousCount =
            targetCount;

        previousRss =
            rss;
    }
}

TEST(
    StorageMemoryTest,
    CapabilityStorageLargeObjectMemoryGrowth)
{
    rim::CapabilityStorage storage;

    const auto rssStart =
        GetVmRSSKb();

    constexpr std::uint32_t kSteps[] =
    {
        100,
        1000,
        5000,
        10000
    };

    std::size_t previousCount = 0;
    std::size_t previousRss = rssStart;

    std::cout
        << "\n=== CapabilityStorage Large Object Memory Growth ===\n";

    for (const auto targetCount : kSteps)
    {
        for (std::uint32_t i =
                static_cast<std::uint32_t>(
                    previousCount);
             i < targetCount;
             ++i)
        {
            LargeCapability capability{};

            capability.data[0] =
                static_cast<std::uint8_t>(
                    i & 0xFF);

            storage.Store(
                static_cast<RICapabilityId>(
                    i + 1),
                capability);
        }

        EXPECT_EQ(
            storage.Size(),
            targetCount);

        const auto rss =
            GetVmRSSKb();

        const auto deltaKb =
            rss - rssStart;

        const auto incrementItems =
            targetCount - previousCount;

        const auto incrementKb =
            rss - previousRss;

        const double totalBytesPerItem =
            targetCount > 0
                ? static_cast<double>(
                    deltaKb * 1024ULL) /
                    static_cast<double>(
                        targetCount)
                : 0.0;

        const double incrementBytesPerItem =
            incrementItems > 0
                ? static_cast<double>(
                    incrementKb * 1024ULL) /
                    static_cast<double>(
                        incrementItems)
                : 0.0;

std::cout
    << "Stored="
    << std::setw(8)
    << storage.Size()

    << " Buckets="
    << std::setw(8)
    << storage.BucketCount()

    << " Load="
    << std::setw(6)
    << std::fixed
    << std::setprecision(2)
    << storage.LoadFactor()

    << " RSS="
    << std::setw(8)
    << rss

    << " KB Delta="
    << std::setw(8)
    << deltaKb

    << " KB PerItem="
    << std::setw(10)
    << totalBytesPerItem

    << " IncPerItem="
    << std::setw(10)
    << incrementBytesPerItem

    << " bytes\n";

        previousCount =
            targetCount;

        previousRss =
            rss;
    }
}

TEST(
    StorageMemoryTest,
    SnapshotStorageMemoryReport)
{
    std::cout
        << "\n=== SnapshotStorage Report ===\n"

        << "StorageSize="
        << sizeof(rim::SnapshotStorage)

        << " EntrySize="
        << rim::SnapshotStorage::EntrySize()

        << " MaxSnapshots="
        << rim::SnapshotStorage::MaxSnapshots()

        << " TotalBytes="
        << rim::SnapshotStorage::EntrySize()
               * rim::SnapshotStorage::MaxSnapshots()

        << '\n';
}

#include <chrono>

TEST(
    StoragePerformanceTest,
    SnapshotStorageStoreThroughput)
{
    rim::SnapshotStorage storage;

    constexpr std::uint32_t kIterations =
        1000000;

    rim::RIMSnapshot snapshot;

    const auto start =
        std::chrono::steady_clock::now();

    for (std::uint32_t i = 0;
         i < kIterations;
         ++i)
    {
        storage.Store(
            snapshot);
    }

    const auto end =
        std::chrono::steady_clock::now();

    const auto elapsedUs =
        std::chrono::duration_cast
        <
            std::chrono::microseconds
        >
        (
            end - start
        )
        .count();

    const auto opsPerSec =
        static_cast<double>(
            kIterations)
        * 1000000.0
        / static_cast<double>(
            elapsedUs);

    std::cout
        << "\n=== Snapshot Store Throughput ===\n"

        << "Operations="
        << kIterations

        << " Time(us)="
        << elapsedUs

        << " Throughput="
        << static_cast<std::uint64_t>(
               opsPerSec)

        << " ops/sec\n";
}

TEST(
    StoragePerformanceTest,
    SnapshotStorageFindThroughput)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    const auto id =
        storage.Store(
            snapshot);

    constexpr std::uint32_t kIterations =
        1000000;

    const auto start =
        std::chrono::steady_clock::now();

    for (std::uint32_t i = 0;
         i < kIterations;
         ++i)
    {
        rim::RIMSnapshot result;

        storage.TryGet(
            id,
            result);
    }

    const auto end =
        std::chrono::steady_clock::now();

    const auto elapsedUs =
        std::chrono::duration_cast
        <
            std::chrono::microseconds
        >
        (
            end - start
        )
        .count();

    const auto opsPerSec =
        static_cast<double>(
            kIterations)
        * 1000000.0
        / static_cast<double>(
            elapsedUs);

    std::cout
        << "\n=== Snapshot Read Throughput ===\n"

        << "Throughput="
        << static_cast<std::uint64_t>(
               opsPerSec)

        << " ops/sec\n";
}

TEST(
    StoragePerformanceTest,
    SnapshotStorageWrapAroundPerformance)
{
    rim::SnapshotStorage storage;

    rim::RIMSnapshot snapshot;

    constexpr std::uint32_t kIterations =
        1000000;

    const auto start =
        std::chrono::steady_clock::now();

    for (std::uint32_t i = 0;
         i < kIterations;
         ++i)
    {
        storage.Store(
            snapshot);
    }

    const auto end =
        std::chrono::steady_clock::now();

    const auto elapsedMs =
        std::chrono::duration_cast
        <
            std::chrono::milliseconds
        >
        (
            end - start
        )
        .count();

    std::cout
        << "\n=== Snapshot WrapAround ===\n"

        << "Stores="
        << kIterations

        << " Time(ms)="
        << elapsedMs
        << '\n';
}

