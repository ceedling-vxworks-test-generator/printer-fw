#pragma once

#include "IRIMSnapshotReader.hpp"
#include "DomainStorageRegistry.hpp"

namespace rim
{

class RIMSnapshotManager
    : public IRIMSnapshotReader
{
public:

    explicit RIMSnapshotManager(
        const DomainStorageRegistry& store);

    RIMSnapshot Read() const override;

private:

    const DomainStorageRegistry&
        store_;
};

}