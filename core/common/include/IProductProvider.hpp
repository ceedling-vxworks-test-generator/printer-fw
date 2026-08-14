#pragma once

#include "../ProductProfile.hpp"
#include "IChangeChecker.hpp"

namespace rim
{

class ProductDefinition;

class IProductProvider
{
public:
    virtual ~IProductProvider() = default;

    virtual const ProductProfile&
    GetProfile() const = 0;
    virtual const IChangeChecker&
    GetChangeChecker() const = 0;
};

} // namespace rim