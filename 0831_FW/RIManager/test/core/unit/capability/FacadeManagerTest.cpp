#include <gtest/gtest.h>

#include "test/core/support/TestDomainInitializer.hpp"

#include "FacadeManager.hpp"

#include "Product.hpp"

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

#include "test/core/support/RIMDataTestHelper.hpp"

namespace
{

class FacadeManagerFixture
    : public ::testing::Test
{
protected:
    rim::PartitionStorageRegistry
        store;

    rim::ProductContext
        productContext;

    FacadeManagerFixture()
        :
        productContext(
            rim::kProductDefinition)
    {
        rim::test::RegisterAllDomains(
            store,
            productContext);
    }
};

rim::RIMDataItem FindFacade(
    const rim::PartitionStorageRegistry& store,
    const rim::ProductContext& context,
    RIFacadeId facadeId)
{
    const auto domainId =
        context.FindDomainId(
            rim::RIMId
            {
                rim::RIMIdType::Facade,
                static_cast<std::uint32_t>(
                    facadeId)
            });
            
    EXPECT_NE(
        domainId,
        rim::kInvalidDomainId);

    const auto* storage =
        store.Find(
            domainId);

    EXPECT_NE(
        storage,
        nullptr);

    rim::RIMDataItem item{};

    EXPECT_TRUE(
        storage->Find(
            static_cast<RIDataId>(
                facadeId),
            item));

    return item;
}

}

using namespace rim::test;

TEST_F(
    FacadeManagerFixture,
    GenerateEnvironmentReadyTrueFromState0)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            0));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot,
            &rim::kEnvironmentReadyFacade));

    const auto facade =
        FindFacade(
            store,
            productContext,
            RI_FACADE_ENVIRONMENT_READY);

    EXPECT_TRUE(
        facade.value.value.b);
}

TEST_F(
    FacadeManagerFixture,
    GenerateEnvironmentReadyTrueFromState1)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            1));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot,
            &rim::kEnvironmentReadyFacade));

    const auto facade =
        FindFacade(
            store,
            productContext,
            RI_FACADE_ENVIRONMENT_READY);

    EXPECT_TRUE(
        facade.value.value.b);
}

TEST_F(
    FacadeManagerFixture,
    GenerateEnvironmentReadyFalseFromState2)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            2));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot,
            &rim::kEnvironmentReadyFacade));

    const auto facade =
        FindFacade(
            store,
            productContext,
            RI_FACADE_ENVIRONMENT_READY);

    EXPECT_FALSE(
        facade.value.value.b);
}

TEST_F(
    FacadeManagerFixture,
    SameEnvironmentReadyValueDoesNotGenerateChange)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            1));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot,
            &rim::kEnvironmentReadyFacade));

    EXPECT_FALSE(
        manager.Evaluate(
            snapshot,
            &rim::kEnvironmentReadyFacade));
}

TEST_F(
    FacadeManagerFixture,
    EnvironmentReadyStateChangeGeneratesChange)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot1;

    snapshot1.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            1));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot1,
            &rim::kEnvironmentReadyFacade));

    rim::RIMSnapshot snapshot2;

    snapshot2.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            2));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot2,
            &rim::kEnvironmentReadyFacade));

    const auto facade =
        FindFacade(
            store,
            productContext,
            RI_FACADE_ENVIRONMENT_READY);

    EXPECT_FALSE(
        facade.value.value.b);
}

TEST_F(
    FacadeManagerFixture,
    MissingEnvironmentCapabilityDoesNotThrow)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    EXPECT_NO_THROW(
        manager.Evaluate(
            snapshot,
            &rim::kEnvironmentReadyFacade));
}

TEST_F(
    FacadeManagerFixture,
    OperationReadyTrueWithEnvironment0AndPrintReadyTrue)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            0));

    snapshot.items.push_back(
        CreateBoolItem(
            RI_CAPABILITY_PRINT_READY,
            true));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot,
            &rim::kOperationReadyFacade));

    const auto facade =
        FindFacade(
            store,
            productContext,
            RI_FACADE_OPERATION_READY);

    EXPECT_TRUE(
        facade.value.value.b);
}

TEST_F(
    FacadeManagerFixture,
    OperationReadyTrueWithEnvironment1AndPrintReadyTrue)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            1));

    snapshot.items.push_back(
        CreateBoolItem(
            RI_CAPABILITY_PRINT_READY,
            true));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot,
            &rim::kOperationReadyFacade));

    const auto facade =
        FindFacade(
            store,
            productContext,
            RI_FACADE_OPERATION_READY);

    EXPECT_TRUE(
        facade.value.value.b);
}

TEST_F(
    FacadeManagerFixture,
    OperationReadyFalseWhenPrintReadyFalse)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            0));

    snapshot.items.push_back(
        CreateBoolItem(
            RI_CAPABILITY_PRINT_READY,
            false));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot,
            &rim::kOperationReadyFacade));

    const auto facade =
        FindFacade(
            store,
            productContext,
            RI_FACADE_OPERATION_READY);

    EXPECT_FALSE(
        facade.value.value.b);
}

TEST_F(
    FacadeManagerFixture,
    OperationReadyFalseWhenEnvironment2)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            2));

    snapshot.items.push_back(
        CreateBoolItem(
            RI_CAPABILITY_PRINT_READY,
            true));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot,
            &rim::kOperationReadyFacade));

    const auto facade =
        FindFacade(
            store,
            productContext,
            RI_FACADE_OPERATION_READY);

    EXPECT_FALSE(
        facade.value.value.b);
}

TEST_F(
    FacadeManagerFixture,
    SameOperationReadyValueDoesNotGenerateChange)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    snapshot.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            1));

    snapshot.items.push_back(
        CreateBoolItem(
            RI_CAPABILITY_PRINT_READY,
            true));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot,
            &rim::kOperationReadyFacade));

    EXPECT_FALSE(
        manager.Evaluate(
            snapshot,
            &rim::kOperationReadyFacade));
}

TEST_F(
    FacadeManagerFixture,
    OperationReadyValueChangeGeneratesChange)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot1;

    snapshot1.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            1));

    snapshot1.items.push_back(
        CreateBoolItem(
            RI_CAPABILITY_PRINT_READY,
            true));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot1,
            &rim::kOperationReadyFacade));

    rim::RIMSnapshot snapshot2;

    snapshot2.items.push_back(
        CreateInt32Item(
            RI_CAPABILITY_ENVIRONMENT,
            2));

    snapshot2.items.push_back(
        CreateBoolItem(
            RI_CAPABILITY_PRINT_READY,
            true));

    EXPECT_TRUE(
        manager.Evaluate(
            snapshot2,
            &rim::kOperationReadyFacade));

    const auto facade =
        FindFacade(
            store,
            productContext,
            RI_FACADE_OPERATION_READY);

    EXPECT_FALSE(
        facade.value.value.b);
}

TEST_F(
    FacadeManagerFixture,
    MissingCapabilitiesForOperationReadyDoesNotThrow)
{
    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    EXPECT_NO_THROW(
        manager.Evaluate(
            snapshot,
            &rim::kOperationReadyFacade));
}
