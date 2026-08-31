#pragma once

#include "Product.hpp"

#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"

#include "PartitionStorageRegistry.hpp"
#include "ProductContext.hpp"

#include "test/core/support/StorageTestHelper.hpp"

namespace rim
{

class TestDataProcessor
{
public:

    static bool StoreData(
        RIDataId id,
        const RIMValue& value,
        PartitionStorageRegistry& store)
    {
        static const ProductContext
            productContext(
                kProductDefinition);

        const auto* definition =
            productContext.FindDataItem(
                {
                    RIMIdType::Data,
                    static_cast<std::uint32_t>(
                        id)
                });

        if (definition == nullptr)
        {
            return false;
        }

        RIMDataItem item{};

        item.id =
            id;

        item.value.type =
            definition->setValueType;

        const RIMValue setValue =
            definition->normalize(
                value,
                nullptr);

        const RIMValue storeValue =
            definition->store(
                RIMValue{},
                setValue,
                nullptr);

        item.value.type =
            definition->storeValueType;

        item.value =
            storeValue;

        const auto domainId =
            productContext.FindDomainId(
                {
                    RIMIdType::Data,
                    static_cast<std::uint32_t>(
                        id)
                });

        if (domainId ==
            kInvalidDomainId)
        {
            return false;
        }

        auto& storageRef =
            rim::test::GetStorage(
                store,
                domainId);

        storageRef.Store(
            item);

        return true;
    }

    static void ProcessTemperatureA(
        double value,
        PartitionStorageRegistry& store)
    {
        StoreData(
            RI_DATA_TEMPERATURE_SENSOR_A,
            RIMValueFactory::CreateDouble(
                value),
            store);
    }

    static void ProcessTemperatureB(
        double value,
        PartitionStorageRegistry& store)
    {
        StoreData(
            RI_DATA_TEMPERATURE_SENSOR_B,
            RIMValueFactory::CreateDouble(
                value),
            store);
    }

    static void ProcessHumidity(
        double value,
        PartitionStorageRegistry& store)
    {
        StoreData(
            RI_DATA_HUMIDITY_SENSOR,
            RIMValueFactory::CreateDouble(
                value),
            store);
    }

    static void ProcessUpperDoor(
        bool value,
        PartitionStorageRegistry& store)
    {
        StoreData(
            RI_DATA_UPPER_DOOR_OPEN,
            RIMValueFactory::CreateBool(
                value),
            store);
    }
};

} // namespace rim