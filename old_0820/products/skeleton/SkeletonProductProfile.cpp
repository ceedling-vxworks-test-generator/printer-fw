#include "SkeletonProductProfile.hpp"

#include "definitions/SkeletonDataDefinitionProvider.hpp"
#include "SkeletonProductDefinition.hpp"

namespace rim
{

static const SkeletonDataDefinitionProvider
kDataDefinitionProvider;

const ProductProfile
kSkeletonProductProfile
{
    kSkeletonProductDefinition,
    &kDataDefinitionProvider,
};

} // namespace rim