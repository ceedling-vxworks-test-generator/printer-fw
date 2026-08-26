#include "../printer_a_capability_id.h"

#include "PrinterAFacadeBuilders.hpp"

#include "RIMSnapshot.hpp"
#include "RIMValueFactory.hpp"

namespace rim
{

RIMValue
BuildEnvironmentReadyFacade(
    const RIMSnapshot& snapshot)
{
    std::int32_t environment{};

    snapshot.TryGetInt32(RI_CAPABILITY_ENVIRONMENT, environment);

    return RIMValueFactory::CreateBool(environment != 2);
}

RIMValue
BuildOperationReadyFacade(
    const RIMSnapshot& snapshot)
{
    std::int32_t environment{};
    bool printReady{};

    snapshot.TryGetInt32(RI_CAPABILITY_ENVIRONMENT, environment);
    snapshot.TryGetBool(RI_CAPABILITY_PRINT_READY, printReady);

    return RIMValueFactory::CreateBool((environment != 2) && printReady);
}

} // namespace rim