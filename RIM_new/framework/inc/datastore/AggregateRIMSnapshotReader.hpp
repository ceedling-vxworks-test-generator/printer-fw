#pragma once

#include "datastore/ValueStore.hpp"
#include "datastore/RIMSnapshot.hpp"

namespace rim
{

class AggregateRIMSnapshotReader
{
public:

    explicit AggregateRIMSnapshotReader(
        const ValueStore& store);

    RIMSnapshot Read() const;

private:

    const ValueStore& store_;
};

}