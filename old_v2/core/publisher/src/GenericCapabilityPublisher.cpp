#include "GenericCapabilityPublisher.hpp"

namespace rim
{

GenericCapabilityPublisher::
GenericCapabilityPublisher(
    PublishFn publishFn,
    void* userData)
    : publishFn_(publishFn)
    , userData_(userData)
{
}

void GenericCapabilityPublisher::Bind(
    PublishFn publishFn,
    void* userData)
{
    publishFn_ = publishFn;
    userData_  = userData;
}

bool GenericCapabilityPublisher::IsBound() const
{
    return publishFn_ != nullptr;
}

void GenericCapabilityPublisher::Publish()
{
    // 未設定でも落とさない(登録されていない Capability は黙って無視する方針)
    if (publishFn_ == nullptr)
    {
        return;
    }

    publishFn_(
        userData_);
}

} // namespace rim
