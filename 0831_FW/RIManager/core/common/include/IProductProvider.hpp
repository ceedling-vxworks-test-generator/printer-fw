#pragma once

#include "../ProductProfile.hpp"
#include "ICustomDeliveryHandler.hpp"

namespace rim
{

class ProductDefinition;

class IProductProvider
{
public:
    virtual ~IProductProvider() = default;

    virtual ICustomDeliveryHandler*
    GetCustomDeliveryHandler()
    {
        return nullptr;
    }

    virtual const ProductProfile&
    GetProfile() const = 0;
};

} // namespace rim