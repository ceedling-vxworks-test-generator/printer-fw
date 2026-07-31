#pragma once

#include "RIMDataId.hpp"
#include "SensorData.hpp"
#include "ValueStore.hpp"

namespace rim
{

class KelvinTemperatureAdapter
{
public:

    bool Store(
        double kelvin,
        ValueStore& store)
    {
        SensorData data{};

        data.id =
            RIMDataId::TemperatureAbsolute;

        data.value =
            kelvin;

        store.Store(data);

        return true;
    }
};

} // namespace rim
