#pragma once

#include "datastore/RIMValue.hpp"
#include "datastore/ValueStore.hpp"

namespace rim
{

class ISensorAdapter
{
public:

    virtual ~ISensorAdapter() = default;

    virtual bool Store(
        const RIMValue& value,
        ValueStore& store) = 0;
};

} // namespace rim