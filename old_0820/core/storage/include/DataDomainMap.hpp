#pragma once

#include <string_view>
#include <unordered_map>

#include "RIId.hpp"
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
        RIDataId dataId) const;

private:

    std::unordered_map<
        RIDataId,
        DomainId>
        map_;
};

} // namespace rim