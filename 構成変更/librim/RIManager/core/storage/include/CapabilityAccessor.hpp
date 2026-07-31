#pragma once

#include "CapabilityId.hpp"
#include "CapabilityPayload.hpp"
#include "CapabilityStore.hpp"

namespace rim
{

//
// CapabilityAccessor - Capability の「現在値」を読み出す窓口。
//
// 旧実装は GetEnvironment() / GetError() / … と Capability ごとに関数を持ち、
// MachineCapabilityStore の具象ゲッタを叩いていた(= Core が製品の Capability
// 一覧を知っている状態)。本実装は CapabilityId を引数で受け、中身は
// CapabilityPayload のまま返す。何が入っているかを知るのは呼出側の責務。
//
// 通知(SubscriberMailbox 経由)が「変化したときだけ届く」のに対し、こちらは
// 「今の値をいつでも読める」経路である。役割が違うので両方残してある。
//
class CapabilityAccessor
{
public:

    explicit CapabilityAccessor(
        const CapabilityStore& store)
        : store_(store)
    {
    }

    // 現在値を型消去のまま取り出す。未生成なら false。
    bool TryGet(
        CapabilityId id,
        CapabilityPayload& payload) const
    {
        return store_.TryGet(
            id,
            payload);
    }

    // 現在値を指定の型として取り出す。
    // 未生成、またはサイズが T と一致しない場合は false。
    //
    // 注意: 検知できるのはサイズ違いだけである(型消去の代償)。
    // CapabilityId と型の対応は Product 側の1箇所に集約すること。
    template <typename T>
    bool TryGetAs(
        CapabilityId id,
        T& out) const
    {
        CapabilityPayload payload;

        if (!store_.TryGet(
                id,
                payload))
        {
            return false;
        }

        const T* typed =
            payload.As<T>();

        if (typed == nullptr)
        {
            return false;
        }

        out = *typed;

        return true;
    }

    bool Has(
        CapabilityId id) const
    {
        return store_.Has(
            id);
    }

private:

    const CapabilityStore&
        store_;
};

} // namespace rim
