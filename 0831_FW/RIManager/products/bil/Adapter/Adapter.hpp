#pragma once

#include "IHardwareAdapter.hpp"

namespace rim
{

class AdapterDispatcher;

class Adapter final
    : public IHardwareAdapter
{
public:

    explicit Adapter(
        AdapterDispatcher& dispatcher);

    bool Initialize() override;

    bool Poll() override;

    void Shutdown() override;

private:

    AdapterDispatcher& dispatcher_;
};

}
