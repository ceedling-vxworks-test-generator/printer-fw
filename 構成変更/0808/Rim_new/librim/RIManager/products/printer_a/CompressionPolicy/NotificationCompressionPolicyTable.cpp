#include "NotificationCompressionPolicyTable.hpp"

#include "rim_capability_id.h"

namespace
{

const rim::NotificationCompressionPolicy
g_policies[] =
{
    {
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_ENVIRONMENT
        },
        rim::CompressionPolicy::KeepLatest
    },

    {
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_CONSUMABLE
        },
        rim::CompressionPolicy::KeepLatest
    },

    {
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_PRINT_READY
        },
        rim::CompressionPolicy::KeepOldest
    },

    {
        {
            rim::NotificationTargetType::Capability,
            RI_CAPABILITY_JOB
        },
        rim::CompressionPolicy::None
    }
};

} // namespace

namespace rim
{

const NotificationCompressionPolicy*
GetNotificationCompressionPolicies()
{
    return g_policies;
}

std::size_t
GetNotificationCompressionPolicyCount()
{
    return sizeof(g_policies)
         / sizeof(g_policies[0]);
}

} // namespace rim