#pragma once

#include <memory>

#include "SensorId.hpp"

#include "ISensorAdapter.hpp"

namespace rim
{

class AdapterFactory
{
public:

    static std::unique_ptr<ISensorAdapter>
    Create(
        SensorId sensorId);
};

} // namespace rim