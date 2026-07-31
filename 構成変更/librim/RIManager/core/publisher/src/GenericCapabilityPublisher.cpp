#include "GenericCapabilityPublisher.hpp"

namespace rim
{

GenericCapabilityPublisher::
GenericCapabilityPublisher(
    PublishFunc publishFunc)
    : publishFunc_(
        std::move(
            publishFunc))
{
}

void GenericCapabilityPublisher::Publish()
{
    publishFunc_();
}

} // namespace rim