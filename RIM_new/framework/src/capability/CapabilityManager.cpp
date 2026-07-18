#include "capability/CapabilityManager.hpp"

namespace rim
{

void CapabilityManager::Evaluate(
    const RIMSnapshot& snapshot)
{
    rules_.Evaluate(
        snapshot,
        store_);
}

} // namespace rim
