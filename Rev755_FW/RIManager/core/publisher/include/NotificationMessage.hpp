#pragma once

#include "NotificationTrigger.hpp"
#include "RIMId.hpp"

namespace rim
{
struct NotificationMessage
{
    RIMId target; //type +id
    NotificationTrigger trigger; // ?????or ????
};
}