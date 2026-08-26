#include <gtest/gtest.h>

#include "FacadeManager.hpp"

#include "PrinterAProductDefinition.hpp"

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

#include "test/support/RIMDataTestHelper.hpp"

namespace
{

rim::RIMDataItem FindFacade(
    const rim::PartitionStorageRegistry& store,
    const rim::ProductContext& context,
    RIFacadeId facadeId)
{
    const auto domainId =
        context.FindFacadeDomainId(
            facadeId);

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

TEST(
    FacadeManagerTest,
    GenerateEnvironmentReadyTrueFromState0)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

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

TEST(
    FacadeManagerTest,
    GenerateEnvironmentReadyTrueFromState1)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

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

TEST(
    FacadeManagerTest,
    GenerateEnvironmentReadyFalseFromState2)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

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

TEST(
    FacadeManagerTest,
    SameEnvironmentReadyValueDoesNotGenerateChange)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

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

TEST(
    FacadeManagerTest,
    EnvironmentReadyStateChangeGeneratesChange)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

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

TEST(
    FacadeManagerTest,
    MissingEnvironmentCapabilityDoesNotThrow)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    EXPECT_NO_THROW(
        manager.Evaluate(
            snapshot,
            &rim::kEnvironmentReadyFacade));
}

TEST(
    FacadeManagerTest,
    OperationReadyTrueWithEnvironment0AndPrintReadyTrue)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

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

TEST(
    FacadeManagerTest,
    OperationReadyTrueWithEnvironment1AndPrintReadyTrue)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

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

TEST(
    FacadeManagerTest,
    OperationReadyFalseWhenPrintReadyFalse)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

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

TEST(
    FacadeManagerTest,
    OperationReadyFalseWhenEnvironment2)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

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

TEST(
    FacadeManagerTest,
    SameOperationReadyValueDoesNotGenerateChange)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

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

TEST(
    FacadeManagerTest,
    OperationReadyValueChangeGeneratesChange)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

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

TEST(
    FacadeManagerTest,
    MissingCapabilitiesForOperationReadyDoesNotThrow)
{
    rim::PartitionStorageRegistry store;

    rim::ProductContext productContext(
        rim::kPrinterAProductDefinition);

    rim::FacadeManager manager(
        store,
        productContext);

    rim::RIMSnapshot snapshot;

    EXPECT_NO_THROW(
        manager.Evaluate(
            snapshot,
            &rim::kOperationReadyFacade));
}
