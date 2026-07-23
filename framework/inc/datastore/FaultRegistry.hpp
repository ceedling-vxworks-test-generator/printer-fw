#pragma once

//
// FaultRegistry - 現在発生中の異常系を保持(仕様 §6.9)。
// apply(item) は context.faultState に応じて発生中Faultを操作し、変化があれば
// kDomainFault を返す(なければ nullopt)。ドメイン単位 mutex。
//

#include <cstdint>
#include <mutex>
#include <optional>

#include "datastore/Config.hpp"
#include "datastore/RIMDataItem.hpp"
#include "datastore/RegistryDomain.hpp"
#include "datastore/Snapshot.hpp"

#include "container/FixedMap.hpp"

namespace rim
{

class FaultRegistry
{
public:

    // FaultState に応じて発生中Faultを操作。変化があれば kDomainFault。
    std::optional<RegistryDomain> Apply(const RIMDataItem& item);

    FaultSnapshot MakeSnapshot() const;

    void Clear();

private:

    struct Entry
    {
        bool active{true};  // true=発生中, false=回復(healed)
    };

    std::uint32_t ResolveKey(const RIMDataItem& item) const;

    mutable std::mutex mutex_;
    FixedMap<std::uint32_t, Entry, kFaultCap> map_;
};

} // namespace rim
