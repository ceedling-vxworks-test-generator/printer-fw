#pragma once

#include <cstddef>
#include <cstdint>

#include "ErrorInfo.hpp"

namespace rim
{

//
// PrinterA の Capability 型。
//
// 以前は core/capability/include/ に置かれていたが、これらは製品固有の語彙であり
// 「Core は製品知識を持たない」に反していたため Product 側へ移した。
// Core へは CapabilityPayload(型消去バイト列)として渡るため、Core はこれらの型を
// 一切知らない。
//
// 制約: CapabilityPayload に格納するため **trivially copyable** であること
//       (std::vector 等を持たせない)。
//

struct EnvironmentCapability
{
    double temperature{};
    double humidity{};
};

struct PrintReadyCapability
{
    bool ready{};
};

struct ConsumableCapability
{
    std::int32_t tonerLevel{};
    std::int32_t stapleLevel{};
};

struct JobCapability
{
    bool         jobActive{};
    std::int32_t jobId{};
};

// 同時に保持する異常の最大数。旧 ErrorCapability は std::vector<ErrorInfo> だったが、
// 動的確保を避けるため固定容量にした(組込み向け: 定常運転中に確保しない)。
// 上限を超えた分は切り捨て、overflow で検知できるようにする。
inline constexpr std::size_t kPrinterAMaxErrors = 16;

struct ErrorCapability
{
    ErrorInfo    errors[kPrinterAMaxErrors]{};
    std::uint16_t count{0};
    bool          overflow{false};   // 上限超過で切り捨てが起きた

    bool Add(const ErrorInfo& info)
    {
        if (count >= kPrinterAMaxErrors) { overflow = true; return false; }
        errors[count++] = info;
        return true;
    }
};

} // namespace rim
