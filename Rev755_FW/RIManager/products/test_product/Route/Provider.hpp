#pragma once

#include "IProductProvider.hpp"
#include "ProductProfile.hpp"

#include "Product.hpp"
#include "DeliveryHandler/CustomDeliveryHandler.hpp"

namespace rim
{

class Provider final
    : public IProductProvider
{
public:

    const ProductProfile&
    GetProfile() const override
    {
        return kProductProfile;
    }
    ICustomDeliveryHandler*
    GetCustomDeliveryHandler() override
    {
        return &customDeliveryHandler_;
    }

private:

    CustomDeliveryHandler
        customDeliveryHandler_;

};

} // namespace rim