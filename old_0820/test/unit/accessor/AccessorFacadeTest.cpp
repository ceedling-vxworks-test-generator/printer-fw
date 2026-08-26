#include <gtest/gtest.h>

#include "AccessorFacade.hpp"

#include "CapabilityAccessor.hpp"
#include "CapabilityStore.hpp"
#include "DataAccessor.hpp"
#include "PrinterAProductDefinition.hpp"
#include "RIMValueFactory.hpp"
#include "SnapshotAccessor.hpp"

#include "DomainStorageRegistry.hpp"
#include "DomainStorage.hpp"
#include "DataDomainMap.hpp"

namespace
{

constexpr RIDataId kUnknownDataId =
    static_cast<RIDataId>(
        99999);

class AccessorFacadeTest :
    public ::testing::Test
{
protected:

    AccessorFacadeTest()
        :
        dataAccessor(
            domainStore,
            rim::kPrinterAProductDefinition),
        capabilityAccessor(
            capabilityStore),
        snapshotAccessor(
            domainStore,
            rim::kPrinterAProductDefinition),
        facade(
            dataAccessor,
            capabilityAccessor),
        dataDomainMap(
            rim::kPrinterAProductDefinition)
    {
    }

    void StoreItem(
        const rim::RIMDataItem& item)
    {
        const auto domainId =
            dataDomainMap.Find(
                item.id);

        ASSERT_NE(
            domainId,
            rim::kInvalidDomainId);

        domainStore
            .GetOrCreate(
                domainId)
            .Store(
                item);
    }

    void
    StoreTemperatureA(
        double value = 25.0)
    {
        StoreItem(
            {
                RI_DATA_TEMPERATURE_SENSOR_A,
                // rim::ValueType::kDouble,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

    void
    StoreTemperatureB(
        double value = 26.0)
    {
        StoreItem(
            {
                RI_DATA_TEMPERATURE_SENSOR_B,
                // rim::ValueType::kDouble,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

    void
    StoreHumidity(
        double value = 50.0)
    {
        StoreItem(
            {
                RI_DATA_HUMIDITY_SENSOR,
                // rim::ValueType::kDouble,
                rim::RIMValueFactory::CreateDouble(
                    value)
            });
    }

    rim::DomainStorageRegistry
        domainStore;

    rim::CapabilityStore
        capabilityStore;

    rim::DataDomainMap
        dataDomainMap;

    rim::DataAccessor
        dataAccessor;

    rim::CapabilityAccessor
        capabilityAccessor;

    rim::SnapshotAccessor
        snapshotAccessor;

    rim::AccessorFacade
        facade;
};

} // namespace

TEST_F(
    AccessorFacadeTest,
    ExposeDataAccessor)
{
    EXPECT_NE(
        &facade.Data(),
        nullptr);
}

TEST_F(
    AccessorFacadeTest,
    ExposeCapabilityAccessor)
{
    EXPECT_NE(
        &facade.Capability(),
        nullptr);
}
