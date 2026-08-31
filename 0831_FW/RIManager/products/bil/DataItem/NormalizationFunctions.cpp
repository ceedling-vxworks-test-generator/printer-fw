#include "NormalizationFunctions.hpp"

namespace rim
{

RIMValue IdentityNormalize(
    const RIMValue& value,
    const void* context)
{
    return value;
}

} // namespace rim
