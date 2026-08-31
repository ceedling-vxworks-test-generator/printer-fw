#pragma once

#include <gtest/gtest.h>

#include "PartitionStorageRegistry.hpp"

namespace rim::test
{

inline PartitionStorage&
GetStorage(
    PartitionStorageRegistry& registry,
    DomainId domainId)
{
    registry.RegisterDomain(
        domainId);

    auto* storage =
        registry.FindMutable(
            domainId);

    EXPECT_NE(
        storage,
        nullptr);

    return *storage;
}

inline const PartitionStorage&
GetStorage(
    const PartitionStorageRegistry& registry,
    DomainId domainId)
{
    const auto* storage =
        registry.Find(
            domainId);

    EXPECT_NE(
        storage,
        nullptr);

    return *storage;
}

} // namespace rim::test