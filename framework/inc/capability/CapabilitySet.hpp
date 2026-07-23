#pragma once

//
// CapabilitySet - 8種Capability(意味づけ結果)の集合。公開契約型として framework に置く。
// 各Capは判定式の適用結果(名詞・再計算可能)。生成されなかったCapは nullopt。
//

#include <cstdint>
#include <optional>

namespace rim
{

struct ErrorCap      { bool hasError{false};  std::uint32_t activeCount{0};
    bool operator==(const ErrorCap& o) const { return hasError == o.hasError && activeCount == o.activeCount; } };
struct JobCap        { bool active{false};    std::uint8_t  progress{0};
    bool operator==(const JobCap& o) const { return active == o.active && progress == o.progress; } };
struct EnvCap        { bool inRange{true};
    bool operator==(const EnvCap& o) const { return inRange == o.inRange; } };
struct MaintCap      { bool needed{false};
    bool operator==(const MaintCap& o) const { return needed == o.needed; } };
struct HealthCap     { bool healthy{true};
    bool operator==(const HealthCap& o) const { return healthy == o.healthy; } };
struct SafetyCap     { bool safe{true};
    bool operator==(const SafetyCap& o) const { return safe == o.safe; } };
struct ConsumableCap { bool inkLow{false};    bool wiperLow{false};
    bool operator==(const ConsumableCap& o) const { return inkLow == o.inkLow && wiperLow == o.wiperLow; } };
struct PrintCap      { bool printable{true};
    bool operator==(const PrintCap& o) const { return printable == o.printable; } };

struct CapabilitySet
{
    std::optional<ErrorCap>      error;
    std::optional<JobCap>        job;
    std::optional<EnvCap>        env;
    std::optional<MaintCap>      maint;
    std::optional<HealthCap>     health;
    std::optional<SafetyCap>     safety;
    std::optional<ConsumableCap> consumable;
    std::optional<PrintCap>      print;
};

} // namespace rim
