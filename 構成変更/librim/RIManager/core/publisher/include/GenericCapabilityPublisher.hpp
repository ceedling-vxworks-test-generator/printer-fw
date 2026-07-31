#pragma once

#include <functional>

#include "ICapabilityPublisher.hpp"

namespace rim
{

class GenericCapabilityPublisher
    : public ICapabilityPublisher
{
public:

    using PublishFunc =
        std::function<void()>;

    explicit GenericCapabilityPublisher(
        PublishFunc publishFunc);

    void Publish() override;

private:

    PublishFunc publishFunc_;
};

} // namespace rim