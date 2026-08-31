#pragma once

#include "IProductProvider.hpp"
#include "ProductProfile.hpp"

#include "Product.hpp"

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
};

} // namespace rim
