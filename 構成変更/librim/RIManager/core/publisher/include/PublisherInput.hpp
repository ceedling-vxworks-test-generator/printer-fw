#pragma once

namespace rim
{

struct PublisherInput
{
    bool environmentChanged = false;

    bool errorChanged = false;

    bool printReadyChanged = false;

    bool consumableChanged = false;

    bool jobChanged = false;

    bool HasAnyChange() const
    {
        return
            environmentChanged ||
            errorChanged ||
            printReadyChanged ||
            consumableChanged ||
            jobChanged;
    }
};

} // namespace rim