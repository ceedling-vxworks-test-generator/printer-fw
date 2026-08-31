#pragma once

namespace rim
{

template<typename T>
class IQueue
{
public:

    virtual ~IQueue() = default;

    virtual bool Push( const T& value) = 0;
    virtual bool TryPop( T& value) = 0;
    virtual bool WaitAndPop( T& value) = 0;
    virtual void Shutdown() = 0;
};

} // namespace rim