#pragma once

#include <unordered_map>
#include <vector>

#include "RIId.hpp"
// #include "DomainId.hpp"
// 

namespace rim
{

struct ProductDefinition;

class FacadeRequiredCapabilityMap
{
public:

    explicit FacadeRequiredCapabilityMap(
        const ProductDefinition& product);

    const std::vector<RICapabilityId>&
    GetRequiredCapabilities(
        RIFacadeId facadeId) const;

private:

    std::unordered_map
    <
        RIFacadeId,
        std::vector<RICapabilityId> // RICapabilityのdmainが実装されてないので暫定。要リファクタ
    >
    map_;
};

} // namespace rim