#pragma once

#include <cstddef>

#include "CapabilityConfig.hpp"
#include "SubscriptionId.hpp"

namespace rim
{

//
// SubscriptionRegistry - 購読 ID の採番と生存管理。
//
// 旧実装は std::unordered_set<SubscriptionId> で、購読のたびにノードを
// 動的確保していた。固定容量の配列へ置き換えて確保を無くしてある。
// 上限は kCapabilitySubscriptionMaxCount。満杯なら採番失敗を返す。
//
class SubscriptionRegistry
{
public:

    // 購読 ID を採番する。満杯なら kInvalidSubscriptionId。
    SubscriptionId Subscribe()
    {
        if (count_ >= kCapabilitySubscriptionMaxCount)
        {
            return kInvalidSubscriptionId;
        }

        const SubscriptionId id = nextId_++;

        subscribers_[count_++] = id;

        return id;
    }

    bool Unsubscribe(
        SubscriptionId id)
    {
        for (std::size_t i = 0; i < count_; ++i)
        {
            if (subscribers_[i] != id)
            {
                continue;
            }

            // 末尾を詰める(順序は保証しない)
            subscribers_[i] = subscribers_[count_ - 1];
            --count_;

            return true;
        }

        return false;
    }

    bool IsSubscribed(
        SubscriptionId id) const
    {
        for (std::size_t i = 0; i < count_; ++i)
        {
            if (subscribers_[i] == id)
            {
                return true;
            }
        }

        return false;
    }

    std::size_t Count() const { return count_; }

private:

    SubscriptionId nextId_{1};

    SubscriptionId subscribers_[kCapabilitySubscriptionMaxCount]{};

    std::size_t count_{0};
};

}
