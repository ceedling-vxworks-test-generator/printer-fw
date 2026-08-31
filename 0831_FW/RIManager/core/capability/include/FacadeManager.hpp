#pragma once

#include "PartitionStorageRegistry.hpp"
#include "RIMSnapshot.hpp"
#include "ProductContext.hpp"

namespace rim
{

class FacadeManager
{
public:

    FacadeManager(
        PartitionStorageRegistry& store,
        const ProductContext& context)
        :
        store_(store),
        context_(context)
    {
    }

    bool Evaluate(
        const RIMSnapshot& snapshot,
        const FacadeItemDefinition* facade);

private:

    PartitionStorageRegistry&
        store_;

    const ProductContext&
        context_;
};

} // namespace rim