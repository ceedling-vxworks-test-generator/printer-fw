#pragma once

#include <unordered_map>
#include <vector>

#include "RIId.hpp"
#include "RIMId.hpp"
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

    const std::vector<
        const FacadeItemDefinition*>&
    Find(
        const RIMId& id) const;

private:

    std::unordered_map<
        RICapabilityId,
        std::vector<
            const FacadeItemDefinition*>>
        map_;
};

} // namespace rim