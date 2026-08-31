#pragma once

#include <gtest/gtest.h>

#include "test/core/support/StorageTestHelper.hpp"

#include "Product.hpp"
#include "RIMValueFactory.hpp"

#include "PartitionStorageRegistry.hpp"
#include "DataDomainMap.hpp"

#include "test/core/support/AccessorTestConstants.hpp"
#include "ProductContext.hpp"

namespace rim::test
{

class AccessorTestFixture
    : public ::testing::Test
{
protected:

    AccessorTestFixture()
        :
        productContext(
            rim::kProductDefinition),
        dataDomainMap(
            rim::kProductDefinition)
    {
    }

    void StoreItem(
        const rim::RIMDataItem& item)
    {
        const rim::RIMId dataId
        {
            rim::RIMIdType::Data,
            static_cast<std::uint32_t>(
                item.id)
        };

        const auto domainId =
            dataDomainMap.Find(
                dataId);

        ASSERT_NE(
            domainId,
            rim::kInvalidDomainId);

        auto& storage =
            rim::test::GetStorage(
                domainStore,
                domainId);

        storage.Store(
            item);
    }

    void StoreTemperatureA(
        double value =
            kTemperatureA)
    {
        StoreItem(
            {
                RI_DATA_TEMPERATURE_SENSOR_A,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

    void StoreTemperatureB(
        double value =
            kTemperatureB)
    {
        StoreItem(
            {
                RI_DATA_TEMPERATURE_SENSOR_B,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

    void StoreHumidity(
        double value =
            kHumidity)
    {
        StoreItem(
            {
                RI_DATA_HUMIDITY_SENSOR,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

    void StoreCapability(
        RICapabilityId capabilityId,
        const rim::RIMValue& value)
    {
        const DomainId domainId =
            productContext.FindDomainId(
                {
                    rim::RIMIdType::Capability,
                    static_cast<std::uint32_t>(
                        capabilityId)
                });
                
        ASSERT_NE(
            domainId,
            kInvalidDomainId);

        auto& storage =
            rim::test::GetStorage(
                domainStore,
                domainId);

        storage.Store(
            {
                static_cast<RIDataId>(
                    capabilityId),
                value
            });
    }

    void StoreEnvironment(
        int32_t state)
    {
        StoreCapability(
            RI_CAPABILITY_ENVIRONMENT,
            rim::RIMValueFactory::CreateInt32(
                state));
    }

    void StorePrintReady(
        bool ready)
    {
        StoreCapability(
            RI_CAPABILITY_PRINT_READY,
            rim::RIMValueFactory::CreateBool(
                ready));
    }

    rim::ProductContext
        productContext;

    rim::PartitionStorageRegistry
        domainStore;

    rim::DataDomainMap
        dataDomainMap;
};

} // namespace rim::test