#pragma once

namespace rim
{

class RIMValue;
struct RIMDataItem;

class IChangeChecker
{
public:

    virtual ~IChangeChecker() = default;

    virtual bool isChanged(const RIMValue* oldValue, const RIMDataItem& item) const = 0;
};

} // namespace rim