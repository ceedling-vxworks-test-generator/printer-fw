#include "SkeletonErrorRegistry.hpp"

namespace rim
{

bool SkeletonErrorRegistry::
TryGetSeverity(
    std::uint32_t,
    ErrorSeverity& severity) const
{
    severity =
        ErrorSeverity::kError;

    return false;
}

} // namespace rim