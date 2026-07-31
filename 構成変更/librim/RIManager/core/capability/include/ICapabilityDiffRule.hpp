#pragma once

#include "CapabilityPayload.hpp"

namespace rim
{

//
// ICapabilityDiffRule - Capability の「変化した」判定規則の抽象。
//
// Core の既定はバイト比較(CapabilityPayload::Equals)。それで足りる Capability は
// 実装不要。誤差許容比較・特定フィールド無視などが要る場合に Product が実装する。
// (変更方針: CapabilityItem は diffRule を持つ)
//
class ICapabilityDiffRule
{
public:

    virtual ~ICapabilityDiffRule() = default;

    virtual bool HasChanged(
        const CapabilityPayload& previous,
        const CapabilityPayload& current) const = 0;
};

} // namespace rim
