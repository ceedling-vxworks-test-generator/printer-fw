#pragma once

#include "IRIMSnapshotReader.hpp"
#include "ValueStore.hpp"

namespace rim
{

class RIMSnapshotManager
    : public IRIMSnapshotReader
{
public:

    explicit RIMSnapshotManager(
        const ValueStore& store);

    RIMSnapshot Read() const override;

private:

    const ValueStore& store_;
};

}