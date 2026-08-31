#pragma once

#include <gtest/gtest.h>

#include "test/core/support/StorageTestHelper.hpp"
#include "test/core/support/TestDomainInitializer.hpp"

#include "CapabilityWorker.hpp"
#include "CapabilityManager.hpp"
#include "FacadeManager.hpp"

#include "CapabilitySnapshotProvider.hpp"
#include "CapabilitySnapshotResolver.hpp"
#include "FacadeSnapshotProvider.hpp"

#include "PartitionStorageRegistry.hpp"
#include "PublisherInputQueue.hpp"
#include "SnapshotAccessor.hpp"

#include "Product.hpp"
#include "ProductContext.hpp"
#include "RIMId.hpp"

class CapabilityWorkerFixture
    : public ::testing::Test
{
protected:

    rim::EventQueue<
        rim::CapabilityInput,
        rim::FifoPolicy>
            queue;

    rim::PublisherInputQueue
        publisherQueue;

    rim::PartitionStorageRegistry
        domainStore;

    rim::ProductContext
        productContext;

    rim::CapabilityManager
        capabilityManager;

    rim::FacadeManager
        facadeManager;

    rim::CapabilitySnapshotResolver
        capabilityResolver;

    rim::SnapshotAccessor
        accessor;

    rim::CapabilitySnapshotProvider
        capabilityProvider;

    rim::FacadeSnapshotProvider
        facadeProvider;

    rim::CapabilityWorker
        worker;

    CapabilityWorkerFixture()
        :
        productContext(
            rim::kProductDefinition),
        capabilityManager(
            domainStore,
            productContext),

        facadeManager(
            domainStore,
            productContext),

        capabilityResolver(
            productContext),

        accessor(
            domainStore,
            productContext),

        capabilityProvider(
            capabilityResolver,
            accessor),

        facadeProvider(
            productContext,
            accessor),

        worker(
            queue,
            capabilityManager,
            facadeManager,
            publisherQueue,
            capabilityProvider,
            facadeProvider,
            productContext)
    {
        rim::test::RegisterAllDomains(
            domainStore,
            productContext);
    }

    rim::PartitionStorage&
    GetPartitionStorage(
        rim::DomainId domainId = 1U)
    {
        return rim::test::GetStorage(
            domainStore,
            domainId);
    }

    void Execute(
        RIDataId changedDataId)
    {
        Execute(
            rim::RIMId{
                rim::RIMIdType::Data,
                static_cast<std::uint32_t>(
                    changedDataId)
            });
    }

    void Execute(
        const rim::RIMId& changedId)
    {
        rim::CapabilityInput input{};

        input.changedId =
            changedId;

        ASSERT_TRUE(
            queue.Push(
                input));

        ASSERT_TRUE(
            worker.ExecuteOnce());
    }
};
