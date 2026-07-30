#pragma once

//
// Snapshot 群 - 読み取り専用・生成後不変・単一時点整合(仕様 §6.11)。
// CurrentValueSnapshot はドメイン別の副Snapshotを optional で束ねる。
//

#include <array>
#include <cstdint>
#include <optional>

#include "datastore/Config.hpp"
#include "datastore/RegistryDomain.hpp"

namespace rim
{

// --- Fault ---
struct FaultSnapshot
{
    std::uint32_t activeCount{0};
    std::array<std::uint32_t, kFaultCap> activeCodes{};  // active(発生中)なコード
    std::uint32_t healedCount{0};                        // healed 状態のコード数
};

// --- Operation ---
struct OperationSnapshot
{
    bool          jobPresent{false};
    std::uint8_t  jobProgress{0};
    bool          jobActive{false};   // 進捗が 1..99 の間 active、100 で完了
};

// --- CurrentValue 副Snapshot(ドメイン別) ---
struct EnvironmentSnapshot
{
    std::optional<std::int32_t> temperatureKelvinX100;  // 絶対温度(ケルビン)×100
    std::optional<std::uint8_t> humidity;
    std::optional<std::uint8_t> pressure;
};

struct ConsumableSnapshot
{
    std::optional<std::uint8_t> inkLevel;
    std::optional<std::uint8_t> wiperLevel;
};

struct SafetySnapshot
{
    std::optional<bool> coverOpen;
    std::optional<bool> eStop;
};

struct MaintenanceSnapshot
{
    std::optional<std::uint32_t> counter;
};

struct HealthSnapshot
{
    std::optional<bool> unitAlive;
};

struct CurrentValueSnapshot
{
    std::optional<EnvironmentSnapshot> environment;
    std::optional<ConsumableSnapshot>  consumable;
    std::optional<SafetySnapshot>      safety;
    std::optional<MaintenanceSnapshot> maintenance;
    std::optional<HealthSnapshot>      health;
};

// --- 集約 ---
struct MachineSnapshot
{
    std::optional<FaultSnapshot>        fault;
    std::optional<OperationSnapshot>    operation;
    std::optional<CurrentValueSnapshot> currentValue;
};

struct SnapshotRequest
{
    RegistryDomainSet domains{kDomainNone};
};

} // namespace rim
