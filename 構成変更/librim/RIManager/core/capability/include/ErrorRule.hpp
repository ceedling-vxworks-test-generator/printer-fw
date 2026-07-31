#pragma once

#include "ErrorCapability.hpp"
#include "ErrorRepository.hpp"

namespace rim
{

class ErrorRule
{
public:

    ErrorCapability Evaluate(
        const ErrorRepository&
            repository) const
    {
        ErrorCapability cap{};

        cap.errors =
            repository.GetAll();

        return cap;
    }
};

}