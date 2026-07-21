#pragma once

//
// CurrentValueRegistry - 最新値系の現在値を保持(仕様 §6.10)。
// apply(item): Id ごとに最新値へ反映。変化があれば該当副ドメインを返す。
// makeSnapshot(domains): 要求ドメインの副Snapshotのみ生成。ドメイン単位 mutex。
//

#include <array>
#include <mutex>
#include <optional>

#include "datastore/RIMDataId.hpp"
#include "datastore/RIMValue.hpp"
#include "datastore/RIMDataItem.hpp"
#include "datastore/RegistryDomain.hpp"
#include "datastore/Snapshot.hpp"

namespace rim
{

class CurrentValueRegistry
{
public:

    std::optional<RegistryDomain> Apply(const RIMDataItem& item);

    // 要求された副ドメインのみ埋めた CurrentValueSnapshot を返す。
    CurrentValueSnapshot MakeSnapshot(RegistryDomainSet domains) const;

    void Clear();

    // Id が属する CurrentValue 副ドメイン(非CurrentValueなら kDomainNone)。
    static RegistryDomain DomainForId(RIMDataId id);

private:

    struct Slot
    {
        RIMValue value{};
        bool     present{false};
    };

    static bool ValueEquals(const RIMValue& a, const RIMValue& b);

    mutable std::mutex mutex_;
    std::array<Slot, kRIMDataIdCount> slots_{};
};

} // namespace rim
