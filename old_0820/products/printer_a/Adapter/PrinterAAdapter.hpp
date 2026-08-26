#pragma once

#include "IHardwareAdapter.hpp"

namespace rim
{

class AdapterDispatcher;

class PrinterAAdapter final
    : public IHardwareAdapter
{
public:

    explicit PrinterAAdapter(
        AdapterDispatcher& dispatcher);

    bool Initialize() override;

    bool Poll() override;

    void Shutdown() override;

private:

    AdapterDispatcher& dispatcher_;
};

}