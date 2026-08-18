#pragma once

#include "IProductProvider.hpp"

#include "../SkeletonProductProfile.hpp"

namespace rim
{

class SkeletonProvider final
    : public IProductProvider
{
public:

    const ProductProfile&
    GetProfile() const override
    {
        return kSkeletonProductProfile;
    }
};

} // namespace rim