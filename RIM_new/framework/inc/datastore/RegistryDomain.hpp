#pragma once

//
// RegistryDomain / RegistryDomainSet - Registry内の保存領域を示すドメイン(ビットマスク)。
// 仕様: notifyUpdated(RegistryDomainSet) / SnapshotRequest.domains に用いる。
// 3 Registry(Fault/Operation/CurrentValue)より細かく、Capability依存に対応した粒度で定義する。
//   Fault       … FaultRegistry
//   Operation   … OperationRegistry
//   Environment / Consumable / Safety / Maintenance / Health … CurrentValueRegistry 内の副ドメイン
//

#include <cstdint>

namespace rim
{

using RegistryDomainSet = std::uint32_t;

enum RegistryDomain : std::uint32_t
{
    kDomainNone        = 0u,
    kDomainFault       = 1u << 0,
    kDomainOperation   = 1u << 1,
    kDomainEnvironment = 1u << 2,
    kDomainConsumable  = 1u << 3,
    kDomainSafety      = 1u << 4,
    kDomainMaintenance = 1u << 5,
    kDomainHealth      = 1u << 6,
};

// CurrentValueRegistry が扱う副ドメイン(まとめて capture 判定に使う)。
constexpr RegistryDomainSet kDomainCurrentAll =
    kDomainEnvironment | kDomainConsumable | kDomainSafety |
    kDomainMaintenance | kDomainHealth;

} // namespace rim
