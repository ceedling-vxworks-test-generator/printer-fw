#pragma once

#include "NotificationTrigger.hpp"
#include "NotificationTarget.hpp"

namespace rim
{
struct NotificationMessage
{
    NotificationTarget target; //type +id
    NotificationTrigger trigger; // ?????or ????
};
}