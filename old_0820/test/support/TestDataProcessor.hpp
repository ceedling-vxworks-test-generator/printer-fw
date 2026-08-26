#pragma once

#include "PrinterAProductDefinition.hpp"

#include "ProductDefinition.hpp"
#include "RIMDataItem.hpp"
#include "RIMValueFactory.hpp"

#include "DomainStorageRegistry.hpp"
#include "DataDomainMap.hpp"

namespace rim
{

class TestDataProcessor
{
public:

    static bool StoreData(
        RIDataId id,
        const RIMValue& value,
        DomainStorageRegistry& store)
    {
        const auto* definition =
            FindDataItem(
                kPrinterAProductDefinition,
                id);

        if (definition == nullptr)
        {
            return false;
        }

        RIMDataItem item{};

        item.id = id;

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

        static const DataDomainMap
            dataDomainMap(
                kPrinterAProductDefinition);

        const auto domainId =
            dataDomainMap.Find(
                id);

        if (domainId ==
            kInvalidDomainId)
        {
            return false;
        }

        store
            .GetOrCreate(
                domainId)
            .Store(
                item);

        return true;
    }

    static void ProcessTemperatureA(
        double value,
        DomainStorageRegistry& store)
    {
        StoreData(
            RI_DATA_TEMPERATURE_SENSOR_A,
            RIMValueFactory::CreateDouble(
                value),
            store);
    }

    static void ProcessTemperatureB(
        double value,
        DomainStorageRegistry& store)
    {
        StoreData(
            RI_DATA_TEMPERATURE_SENSOR_B,
            RIMValueFactory::CreateDouble(
                value),
            store);
    }

    static void ProcessHumidity(
        double value,
        DomainStorageRegistry& store)
    {
        StoreData(
            RI_DATA_HUMIDITY_SENSOR,
            RIMValueFactory::CreateDouble(
                value),
            store);
    }

    static void ProcessUpperDoor(
        bool value,
        DomainStorageRegistry& store)
    {
        StoreData(
            RI_DATA_UPPER_DOOR_OPEN,
            RIMValueFactory::CreateBool(
                value),
            store);
    }
};

} // namespace rim