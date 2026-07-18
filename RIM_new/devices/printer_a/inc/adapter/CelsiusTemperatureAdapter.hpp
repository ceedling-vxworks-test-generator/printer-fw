#pragma once

#include "datastore/RIMDataId.hpp"
#include "datastore/SensorData.hpp"
#include "datastore/ValueStore.hpp"

namespace rim
{

class CelsiusTemperatureAdapter
{
public:

    static constexpr double kKelvinOffset =
        273.15;

    bool Store(
        double celsius,
        ValueStore& store)
    {
        SensorData data{};

        data.id =
            RIMDataId::TemperatureAbsolute;

        data.value =
            celsius + kKelvinOffset;

        store.Store(data);

        return true;
    }
};

} // namespace rim
