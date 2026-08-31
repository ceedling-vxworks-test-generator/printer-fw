#pragma once

#include <unordered_map>

#include "RIMId.hpp"
#include "DomainId.hpp"

namespace rim
{

struct ProductDefinition;

class DataDomainMap
{
public:

    explicit DataDomainMap(
        const ProductDefinition& product);

    DomainId Find(
        const RIMId& id) const;

private:

    std::unordered_map<
        RIMId,
        DomainId,
        RIMIdHash>
            map_;
};

} // namespace rim