#pragma once

#include "IDataDefinitionProvider.hpp"
#include "IErrorDefinitionRegistry.hpp"

namespace rim
{

class ProductDefinition;

class IProductProvider
{
public:

    virtual ~IProductProvider() = default;

    virtual const IErrorDefinitionRegistry&
    GetErrorRegistry() const = 0;

    virtual const IDataDefinitionProvider&
    GetDataDefinitionProvider() const = 0;

    virtual const ProductDefinition&
    GetProductDefinition() const = 0;
};

} // namespace rim
