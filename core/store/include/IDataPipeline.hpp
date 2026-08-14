#pragma once

namespace rim
{

class RIMDataItem;

class IDataPipeline
{
public:

    virtual ~IDataPipeline() = default;
    virtual bool push(const RIMDataItem& item) = 0;

};

} // namespace rim