#pragma once

#include <memory>

#include "datastore/SensorId.hpp"

#include "adapter/ISensorAdapter.hpp"

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