#pragma once

namespace rim
{

class ICapabilityPublisher
{
public:

    virtual ~ICapabilityPublisher() = default;

    virtual void Publish() = 0;
};

} // namespace rim