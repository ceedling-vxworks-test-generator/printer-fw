#include <gtest/gtest.h>

#include <array>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "CapabilityStorage.hpp"
#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"
#include "DomainStorageRegistry.hpp"
#include "DomainStorage.hpp"
#include "DataDomainMap.hpp"
#include "PrinterAProductDefinition.hpp"

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
        << "DomainStorage     : "
        << sizeof(rim::DomainStorage)
        << '\n';

    std::cout
        << "DomainRegistry    : "
        << sizeof(rim::DomainStorageRegistry)
        << '\n';

    std::cout
        << "CapabilityStorage : "
        << sizeof(rim::CapabilityStorage)
        << '\n';

    std::cout
        << "RIMSnapshot       : "
        << sizeof(rim::RIMSnapshot)
        << '\n';
}

// TEST(
//     StorageMemoryTest,
//     DomainStorageMemoryGrowth)
// {
//     rim::DomainStorageRegistry store;

//     rim::DataDomainMap domainMap(
//         rim::kPrinterAProductDefinition);

//     const auto rssStart =
//         GetVmRSSKb();

//     constexpr std::uint32_t kSteps[] =
//     {
//         100,
//         1000,
//         10000,
//         50000,
//         100000,
//         1000000,
//         2000000
//     };

//     std::uint32_t currentCount = 0;

//     std::cout
//         << "\n=== DomainStorage Memory Growth ===\n";

//     for (const auto targetCount : kSteps)
//     {
//         for (; currentCount < targetCount; ++currentCount)
//         {
//             rim::RIMDataItem item{};

//             item.id =
//                 static_cast<RIDataId>(
//                     currentCount + 1);

//             item.valueType =
//                 rim::ValueType::kInt32;

//             item.value =
//                 rim::RIMValueFactory::CreateInt32(
//                     static_cast<std::int32_t>(
//                         currentCount));

//             const auto domainId =
//                 domainMap.Find(
//                     item.id);

//             ASSERT_NE(
//                 domainId,
//                 rim::kInvalidDomainId);

//             store
//                 .GetOrCreate(
//                     domainId)
//                 .Store(
//                     item);
//         }

//         const auto rss =
//             GetVmRSSKb();

//         std::size_t itemCount = 0;

//         for (DomainId domainId = 1;
//             domainId < 64;
//             ++domainId)
//         {
//             const auto* storage =
//                 store.Find(
//                     domainId);

//             if (storage == nullptr)
//             {
//                 continue;
//             }

//             itemCount +=
//                 storage->GetAll().size();
//         }

//         EXPECT_EQ(
//             itemCount,
//             targetCount);

//         const auto deltaKb =
//             rss - rssStart;

//         const double bytesPerItem =
//             itemCount > 0
//                 ? static_cast<double>(
//                     deltaKb * 1024ULL) /
//                     static_cast<double>(
//                         itemCount)
//                 : 0.0;

//         std::cout
//             << "Stored="
//             << std::setw(8)
//             << itemCount

//             << " RSS="
//             << std::setw(8)
//             << rss

//             << " KB Delta="
//             << std::setw(8)
//             << deltaKb

//             << " KB PerItem="
//             << std::setw(10)
//             << std::fixed
//             << std::setprecision(2)
//             << bytesPerItem

//             << " bytes\n";
//     }
// }

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
