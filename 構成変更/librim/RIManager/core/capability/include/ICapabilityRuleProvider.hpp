#pragma once

#include <cstddef>

#include "CapabilityEvaluator.hpp"

namespace rim
{

//
// ICapabilityRuleProvider - Product が持つ Capability 規則一式を Core へ渡す IF。
//
// Core は「規則が何個あってどんな Capability を作るか」を知らない。知っているのは
// Product だけであり、Product はこの IF を実装して自分の規則を評価器へ登録する。
//
// 依存方向: core は本 IF の**宣言**しか持たない。実装は products/ 側にあり、
// リンク時に解決される(RIMDataId.hpp のヘッダ注入と同じ考え方)。
//
class ICapabilityRuleProvider
{
public:

    virtual ~ICapabilityRuleProvider() = default;

    // 自分が持つ規則を評価器へ登録する。
    virtual void RegisterTo(CapabilityEvaluator& evaluator) const = 0;

    // この Product が定義する Capability の数(CapabilityId の上限)。
    // Core は診断用にしか使わない。
    virtual std::size_t CapabilityCount() const = 0;
};

} // namespace rim
