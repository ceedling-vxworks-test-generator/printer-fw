#pragma once

#include "datastore/RIMDataId.hpp"
#include "datastore/SensorData.hpp"
#include "datastore/ValueStore.hpp"

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
