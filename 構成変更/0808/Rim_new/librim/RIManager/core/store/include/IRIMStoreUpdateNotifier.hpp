#pragma once

namespace rim
{

class IRIMStoreUpdateNotifier
{
public:

    virtual ~IRIMStoreUpdateNotifier() = default;
    virtual void notifyUpdated(const RIMDataItem& domitemains) = 0;
};

}