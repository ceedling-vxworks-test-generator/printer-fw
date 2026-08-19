#pragma once

#include "CapabilityStore.hpp"
#include "PrinterAProductDefinition.hpp"
#include "RIMValueFactory.hpp"
#include "SnapshotStorage.hpp"

#include "DomainStorageRegistry.hpp"
#include "DataDomainMap.hpp"

#include "test/support/AccessorTestConstants.hpp"

namespace rim::test
{

class AccessorTestFixture
{
protected:

    AccessorTestFixture()
        :
        dataDomainMap(
            rim::kPrinterAProductDefinition)
    {
    }

    void
    StoreItem(
        const rim::RIMDataItem& item)
    {
        const auto domainId =
            dataDomainMap.Find(
                item.id);

        if (domainId ==
            rim::kInvalidDomainId)
        {
            return;
        }

        domainStore
            .GetOrCreate(
                domainId)
            .Store(
                item);
    }

    void
    StoreTemperatureA(
        double value =
            kTemperatureA)
    {
        StoreItem(
            {
                RI_DATA_TEMPERATURE_SENSOR_A,
                rim::ValueType::kDouble,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

    void
    StoreTemperatureB(
        double value =
            kTemperatureB)
    {
        StoreItem(
            {
                RI_DATA_TEMPERATURE_SENSOR_B,
                rim::ValueType::kDouble,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

    void
    StoreHumidity(
        double value =
            kHumidity)
    {
        StoreItem(
            {
                RI_DATA_HUMIDITY_SENSOR,
                rim::ValueType::kDouble,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

protected:

    rim::DomainStorageRegistry
        domainStore;

    rim::DataDomainMap
        dataDomainMap;

    rim::CapabilityStore
        capabilityStore;

    rim::SnapshotStorage
        snapshotStorage;
};

} // namespace rim::test