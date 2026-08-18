#pragma once

#include <unordered_map>
#include <vector>

#include "rim_data_id.h"

#include "CapabilityItem/CapabilityItemDefinition.hpp"

namespace rim
{

struct ProductDefinition;

class CapabilityDependencyMap
{
public:

    explicit CapabilityDependencyMap(
        const ProductDefinition& product);

    const std::vector<
        const CapabilityItemDefinition*>&
    Find(
        RIDataId dataId) const;

private:

    std::unordered_map<
        RIDataId,
        std::vector<
            const CapabilityItemDefinition*>>
        map_;
};

} // namespace rim