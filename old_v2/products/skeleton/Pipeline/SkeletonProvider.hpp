#pragma once

#include "IProductProvider.hpp"

#include "../definitions/SkeletonDataDefinitionProvider.hpp"
#include "../definitions/SkeletonErrorRegistry.hpp"

namespace rim
{

class SkeletonProvider final
    : public IProductProvider
{
public:

    const IErrorDefinitionRegistry&
    GetErrorRegistry() const override
    {
        return errorRegistry_;
    }

    const IDataDefinitionProvider&
    GetDataDefinitionProvider() const override
    {
        return dataDefinitionProvider_;
    }

private:

    SkeletonErrorRegistry
        errorRegistry_;

    SkeletonDataDefinitionProvider
        dataDefinitionProvider_;
};

} // namespace rim