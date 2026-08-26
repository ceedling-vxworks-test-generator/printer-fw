#include "DataDomainMap.hpp"

#include "ProductDefinition.hpp"
#include "DataItemDefinition.hpp"

#include <unordered_map>

#include "DomainId.hpp"

namespace rim
{

    DataDomainMap::DataDomainMap(
        const ProductDefinition& product)
    {
        std::unordered_map<
            const DomainDefinition*,
            DomainId>
            domains;

        DomainId nextId = 1;

        for (std::size_t i = 0;
            i < product.dataItemCount;
            ++i)
        {
            const auto& item =
                product.dataItems[i];

            const auto it =
                domains.find(
                    item.domain);

            DomainId domainId =
                kInvalidDomainId;

            if (it == domains.end())
            {
                domainId =
                    nextId++;

                domains.emplace(
                    item.domain,
                    domainId);
            }
            else
            {
                domainId =
                    it->second;
            }

            map_[item.id] =
                domainId;
        }
    }

    DomainId
    DataDomainMap::Find(
        RIDataId dataId) const
    {
        const auto it =
            map_.find(dataId);

        if (it == map_.end())
        {
            return kInvalidDomainId;
        }

        return it->second;
    }

} // namespace rim
