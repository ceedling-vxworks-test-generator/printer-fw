#include <gtest/gtest.h>

#include <thread>
#include <vector>

#include "rim_api.h"

#include "PartitionStorage.hpp"

namespace rim
{

namespace
{

RIMDataItem CreateItem(
    RIDataId id,
    std::int32_t value)
{
    RIMDataItem item{};

    item.id = id;

    // item.valueType =
    //     ValueType::kInt32;

    item.value.type =
        ValueType::kInt32;

    item.value.value.i32 =
        value;

    return item;
}

constexpr RIDataId kTestIds[] =
{
    RI_DATA_TEMPERATURE_SENSOR_A,
    RI_DATA_TEMPERATURE_SENSOR_B,
    RI_DATA_HUMIDITY_SENSOR,
    RI_DATA_UPPER_DOOR_OPEN,
    RI_DATA_RIGHT_DOOR_OPEN,
    RI_DATA_LEFT_DOOR_OPEN,
    RI_DATA_STAPLE_LEVEL,
    RI_DATA_TONER_LEVEL
};

} // namespace

TEST(
    PartitionStorageThreadTest,
    ConcurrentWrite)
{
    PartitionStorage storage;

    constexpr std::size_t
        kThreadCount = 8;

    constexpr std::size_t
        kIterations = 1000;

    std::vector<std::thread>
        threads;

    for (std::size_t t = 0;
         t < kThreadCount;
         ++t)
    {
        threads.emplace_back(
            [&storage]()
{
                for (std::size_t i = 0;
                     i < kIterations;
                     ++i)
                {
                    storage.Store(
                        CreateItem(
                            kTestIds[
                                i %
                                std::size(
                                    kTestIds)],
                            static_cast<
                                std::int32_t>(
                                    i)));
                }
            });
    }

    for (auto& thread
         : threads)
    {
        thread.join();
    }
}

TEST(
    PartitionStorageThreadTest,
    ConcurrentRead)
{
    PartitionStorage storage;

    for (const auto id :
         kTestIds)
    {
        storage.Store(
            CreateItem(
                id,
                123));
    }

    constexpr std::size_t
        kThreadCount = 16;

    std::vector<std::thread>
        threads;

    for (std::size_t i = 0;
         i < kThreadCount;
         ++i)
    {
        threads.emplace_back(
            [&]()
            {
                for (std::size_t n = 0;
                     n < 10000;
                     ++n)
                {
                    RIMDataItem out{};

                    storage.Find(
                        kTestIds[
                            n %
                            std::size(
                                kTestIds)],
                        out);
                }
            });
    }

    for (auto& thread
         : threads)
    {
        thread.join();
    }

    SUCCEED();
}

TEST(
    PartitionStorageThreadTest,
    ReadWrite)
{
    PartitionStorage storage;

    constexpr std::size_t
        kIterations = 10000;

    std::thread writer(
        [&]()
        {
            for (std::size_t i = 0;
                 i < kIterations;
                 ++i)
            {
                storage.Store(
                    CreateItem(
                        kTestIds[
                            i %
                            std::size(
                                kTestIds)],
                        static_cast<
                            std::int32_t>(
                                i)));
            }
        });

    std::thread reader(
        [&]()
        {
            for (std::size_t i = 0;
                 i < kIterations;
                 ++i)
            {
                RIMDataItem out{};

                storage.Find(
                    kTestIds[
                        i %
                        std::size(
                            kTestIds)],
                    out);
            }
        });

    writer.join();
    reader.join();
}

} // namespace rim