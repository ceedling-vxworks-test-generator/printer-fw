#pragma once

#include "IRIMSnapshotReader.hpp"
#include "ValueStore.hpp"

namespace rim
{

class AggregateRIMSnapshotReader
    : public IRIMSnapshotReader
{
public:

    explicit AggregateRIMSnapshotReader(
        const ValueStore& store);

    RIMSnapshot Read() const override;

private:

    const ValueStore& store_;
};

}