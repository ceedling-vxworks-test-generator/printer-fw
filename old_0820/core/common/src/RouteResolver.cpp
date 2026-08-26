#include "RouteResolver.hpp"

#include "ProductDefinition.hpp"
#include "CapabilityItemDefinition.hpp"
#include "DataItemDefinition.hpp"
#include "RouteDefinition.hpp"

namespace rim
{

const RouteDefinition*
RouteResolver::Resolve(
    const ProductDefinition& product,
    const NotificationTarget& target)
{
    switch (target.type)
    {
    case NotificationTargetType::Capability:
    {
        const auto* capability =
            FindCapability(
                product,
                static_cast<RICapabilityId>(
                    target.id));

        if (capability == nullptr)
        {
            return nullptr;
        }

        return FindRoute(
            product,
            capability->route);
    }

    case NotificationTargetType::Data:
    {
        const auto* dataItem =
            FindDataItem(
                product,
                static_cast<RIDataId>(
                    target.id));

        if (dataItem == nullptr)
        {
            return nullptr;
        }

        return FindRoute(
            product,
            dataItem->route);
    }

    case NotificationTargetType::Facade:
    {
        //TODO Facadeが追加されたら実装
    }

    default:
        return nullptr;
    }
}

} // namespace rim