#include "capability/PrinterACapabilityBuilder.hpp"

namespace rim
{

namespace
{
// 閾値(PrinterA固有・TBDを妥当値で確定)。
constexpr std::int32_t  kTempAlertKelvinX100 = 33315;  // 60.00℃ = 333.15K
constexpr std::uint8_t  kHumidityLow      = 20;
constexpr std::uint8_t  kHumidityHigh     = 80;
constexpr std::uint8_t  kConsumableLow    = 10;     // %
constexpr std::uint32_t kMaintThreshold   = 10000;  // カウンタ
} // namespace

CapabilitySet PrinterACapabilityBuilder::Build(const MachineSnapshot& s) const
{
    CapabilitySet cap;

    // --- ErrorCap (FaultRegistry) ---
    {
        ErrorCap c;
        if (s.fault) {
            c.activeCount = s.fault->activeCount;
            c.hasError    = s.fault->activeCount > 0;
        }
        cap.error = c;
    }

    // --- JobCap (OperationRegistry) ---
    {
        JobCap c;
        if (s.operation) {
            c.active   = s.operation->jobActive;
            c.progress = s.operation->jobProgress;
        }
        cap.job = c;
    }

    // 現在値系
    const bool hasCur = s.currentValue.has_value();
    const EnvironmentSnapshot* env = hasCur && s.currentValue->environment ? &*s.currentValue->environment : nullptr;
    const ConsumableSnapshot*  con = hasCur && s.currentValue->consumable  ? &*s.currentValue->consumable  : nullptr;
    const SafetySnapshot*      saf = hasCur && s.currentValue->safety      ? &*s.currentValue->safety      : nullptr;
    const MaintenanceSnapshot* mnt = hasCur && s.currentValue->maintenance ? &*s.currentValue->maintenance : nullptr;
    const HealthSnapshot*      hlt = hasCur && s.currentValue->health      ? &*s.currentValue->health      : nullptr;

    // --- EnvCap (CurrentValueRegistry: 温湿度範囲) ---
    {
        EnvCap c;
        bool inRange = true;
        if (env) {
            if (env->temperatureKelvinX100 && *env->temperatureKelvinX100 > kTempAlertKelvinX100) inRange = false;
            if (env->humidity && (*env->humidity < kHumidityLow || *env->humidity > kHumidityHigh)) inRange = false;
        }
        c.inRange = inRange;
        cap.env = c;
    }

    // --- ConsumableCap (インク/ワイパー残量) ---
    {
        ConsumableCap c;
        if (con) {
            if (con->inkLevel)   c.inkLow   = *con->inkLevel   < kConsumableLow;
            if (con->wiperLevel) c.wiperLow = *con->wiperLevel < kConsumableLow;
        }
        cap.consumable = c;
    }

    // --- SafetyCap (カバー/E-Stop) ---
    {
        SafetyCap c;
        bool safe = true;
        if (saf) {
            if (saf->coverOpen && *saf->coverOpen) safe = false;
            if (saf->eStop && *saf->eStop)         safe = false;
        }
        c.safe = safe;
        cap.safety = c;
    }

    // --- MaintCap (メンテ要否) ---
    {
        MaintCap c;
        if (mnt && mnt->counter) c.needed = *mnt->counter > kMaintThreshold;
        cap.maint = c;
    }

    // --- HealthCap (ユニット死活/劣化) ---
    {
        HealthCap c;
        bool healthy = true;
        if (hlt && hlt->unitAlive) healthy = *hlt->unitAlive;
        if (s.fault && s.fault->activeCount > 0) healthy = false;
        c.healthy = healthy;
        cap.health = c;
    }

    // --- PrintCap (印字可能性: DataStore値のみに依存) ---
    {
        PrintCap c;
        bool printable = true;
        if (s.fault && s.fault->activeCount > 0) printable = false;
        if (saf) {
            if (saf->coverOpen && *saf->coverOpen) printable = false;
            if (saf->eStop && *saf->eStop)         printable = false;
        }
        if (con && con->inkLevel && *con->inkLevel == 0) printable = false;
        c.printable = printable;
        cap.print = c;
    }

    return cap;
}

} // namespace rim
