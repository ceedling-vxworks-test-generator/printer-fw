#pragma once

#include "IChangeChecker.hpp"

namespace rim
{

class RIMValue;

class ChangeChecker : public IChangeChecker
{
public:

    ChangeChecker() = default;
    bool isChanged(const RIMValue* oldValue, const RIMDataItem& item) const override;

};

} // namespace printer