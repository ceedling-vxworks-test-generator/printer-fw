#include <unordered_map>
#include "CompressionPolicyResolver.hpp"
#include "CompressionPolicy/NotificationCompressionPolicyTable.hpp"
#include "NotificationTargetKey.hpp"

namespace
{

    std::unordered_map<
        std::uint64_t,
        rim::CompressionPolicy>
    CreatePolicyMap()
    {
        std::unordered_map<
            std::uint64_t,
            rim::CompressionPolicy>
            map;

        const auto* policies =
            rim::
            GetNotificationCompressionPolicies();

        const auto count =
            rim::
            GetNotificationCompressionPolicyCount();

        for (std::size_t i = 0;
            i < count;
            ++i)
        {
            map.emplace(
                rim::MakeNotificationTargetKey(
                    policies[i].target),
                policies[i].policy);
        }

        return map;
    }

    const auto g_policyMap =
        CreatePolicyMap();

}

namespace rim
{

    CompressionPolicy
    CompressionPolicyResolver::Resolve(
        const NotificationTarget& target)
    {
        auto it =
            g_policyMap.find(
                MakeNotificationTargetKey(
                    target));

        if (it ==
            g_policyMap.end())
        {
            return CompressionPolicy::None;
        }

        return it->second;
    }

} // namespace rim