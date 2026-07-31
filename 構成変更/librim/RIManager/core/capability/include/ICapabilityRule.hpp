#pragma once

#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"
#include "RIMSnapshot.hpp"

namespace rim
{

//
// ICapabilityRule - Snapshot から Capability を生成する規則の抽象。
//
// 実装は **Product** 側に置く(EnvironmentRule / PrintReadyRule など)。
// Core はこの IF 越しにしか Capability 生成を呼ばないため、どんな Capability が
// あるか・中身が何かを知らずに済む。
//
class ICapabilityRule
{
public:

    virtual ~ICapabilityRule() = default;

    // この規則が生成する Capability の識別子。
    virtual CapabilityId Id() const = 0;

    // Snapshot から Capability を生成する。
    // 生成できない(必要な DataItem が未取得等)場合は false を返し、out は変更しない。
    virtual bool Evaluate(
        const RIMSnapshot& snapshot,
        CapabilityPayload& out) const = 0;
};

} // namespace rim
