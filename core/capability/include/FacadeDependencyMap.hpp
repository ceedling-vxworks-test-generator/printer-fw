#pragma once

#include <unordered_map>
#include <vector>

#include "RIId.hpp"
#include "FacadeItemDefinition.hpp"

namespace rim
{

struct ProductDefinition;

class FacadeDependencyMap
{
public:

    explicit FacadeDependencyMap(
        const ProductDefinition& product);

    const std::vector<
        const FacadeItemDefinition*>&
    Find(
        RICapabilityId capabilityId) const;

private:

    std::unordered_map<
        RICapabilityId,
        std::vector<
            const FacadeItemDefinition*>>
        map_;
};

} // namespace rim