#pragma once

#include <gtest/gtest.h>

#include "CapabilityWorker.hpp"
#include "CapabilityManager.hpp"
#include "FacadeManager.hpp"

#include "CapabilitySnapshotProvider.hpp"
#include "CapabilitySnapshotResolver.hpp"
#include "FacadeSnapshotProvider.hpp"

#include "PartitionStorageRegistry.hpp"
#include "PublisherInputQueue.hpp"
#include "SnapshotAccessor.hpp"

#include "PrinterAProductDefinition.hpp"
#include "ProductContext.hpp"

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
            rim::kPrinterAProductDefinition),

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
            domainStore),

        worker(
            queue,
            capabilityManager,
            facadeManager,
            publisherQueue,
            capabilityProvider,
            facadeProvider,
            productContext)
    {
    }

    rim::PartitionStorage&
    GetPartitionStorage(
        std::uint32_t domainId = 1U)
    {
        return domainStore.GetOrCreate(
            domainId);
    }

    void Execute(
        RIDataId changedDataId)
    {
        rim::CapabilityInput input{};

        input.changedDataId =
            changedDataId;

        ASSERT_TRUE(
            queue.Push(
                input));

        ASSERT_TRUE(
            worker.ExecuteOnce());
    }
};