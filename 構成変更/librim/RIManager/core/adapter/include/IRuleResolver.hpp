#pragma once

//
// IRuleResolver - 識別子に応じて適用する Rule を決定する(仕様 §6)。devices が実装する。
//

#include "RIMDataId.hpp"
#include "IRule.hpp"

namespace rim
{

class IRuleResolver
{
public:

    virtual ~IRuleResolver() = default;

    // id に対応する Rule。無ければ nullptr。
    virtual const IRule* SelectRule(RIMDataId id) const = 0;
};

} // namespace rim
