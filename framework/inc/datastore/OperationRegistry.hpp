#pragma once

//
// OperationRegistry - 現在の動作報告を保持(仕様 §6.9 二つ目)。
// apply(item): JobProgress を反映。変化があれば kDomainOperation。ドメイン単位 mutex。
//

#include <cstdint>
#include <mutex>
#include <optional>

#include "datastore/RIMDataItem.hpp"
#include "datastore/RegistryDomain.hpp"
#include "datastore/Snapshot.hpp"

namespace rim
{

class OperationRegistry
{
public:

    std::optional<RegistryDomain> Apply(const RIMDataItem& item);

    OperationSnapshot MakeSnapshot() const;

    void Clear();

private:

    mutable std::mutex mutex_;
    bool          present_{false};
    std::uint8_t  jobProgress_{0};
};

} // namespace rim
