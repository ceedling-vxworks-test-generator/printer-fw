#pragma once

#include <unordered_map>
#include <vector>

#include "RIMId.hpp"
#include "CapabilityItemDefinition.hpp"

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
        const RIMId& id) const;

private:

    std::unordered_map<
        RIMId,
        std::vector<
            const CapabilityItemDefinition*>,
        RIMIdHash>
        map_;
};

} // namespace rim