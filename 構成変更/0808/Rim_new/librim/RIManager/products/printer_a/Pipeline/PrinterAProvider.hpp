#pragma once

#include "IProductProvider.hpp"
#include "../PrinterAProductProfile.hpp"

#include "../PrinterAProductDefinition.hpp"
#include "../ChangeChecker.hpp"

namespace rim
{

class PrinterAProvider final
    : public IProductProvider
{
public:

    const ProductProfile&
    GetProfile() const override
    {
        return kPrinterAProductProfile;
    }

    const IChangeChecker&
    GetChangeChecker() const override
    {
        return changeChecker_;
    }

private:

    ChangeChecker
        changeChecker_;

};

} // namespace rim