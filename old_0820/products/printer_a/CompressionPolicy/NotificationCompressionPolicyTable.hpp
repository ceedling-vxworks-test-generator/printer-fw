#pragma once

#include <cstddef>

#include "NotificationCompressionPolicy.hpp"

namespace rim
{

const NotificationCompressionPolicy*
GetNotificationCompressionPolicies();

std::size_t
GetNotificationCompressionPolicyCount();

} // namespace rim