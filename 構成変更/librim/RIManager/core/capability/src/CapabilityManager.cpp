#include "CapabilityManager.hpp"

namespace rim
{

void CapabilityManager::Evaluate(
    const RIMSnapshot& snapshot)
{
    store_.Store(
        environmentRule_.Evaluate(
            snapshot));

    store_.Store(
        errorRule_.Evaluate(
            errorRepository_));

    store_.Store(
        printReadyRule_.Evaluate(
            snapshot,
            errorRepository_));

    store_.Store(
        consumableRule_.Evaluate(
            snapshot));

    store_.Store(
        jobRule_.Evaluate(
            snapshot));

}

} // namespace rim