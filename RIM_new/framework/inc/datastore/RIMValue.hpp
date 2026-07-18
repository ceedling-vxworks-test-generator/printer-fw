#pragma once

//
// RIMValue - 正規化後の標準値。C表現のタグ付きunion(rim_data_value_t 相当)。
// type タグは手動管理(生成側が設定)。
//

#include <cstdint>

#include "datastore/ValueType.hpp"

namespace rim
{

struct RIMValue
{
    ValueType type{ValueType::kNone};

    union
    {
        std::int32_t  celsiusX100;
        std::uint8_t  percent;
        std::uint8_t  jobProgress;
        std::uint32_t faultCode;
    } u{};

    // --- 生成ヘルパ ---
    static RIMValue CelsiusX100(std::int32_t v)
    {
        RIMValue r;
        r.type = ValueType::kCelsiusX100;
        r.u.celsiusX100 = v;
        return r;
    }

    static RIMValue Percent(std::uint8_t v)
    {
        RIMValue r;
        r.type = ValueType::kPercent;
        r.u.percent = v;
        return r;
    }

    static RIMValue JobProgress(std::uint8_t v)
    {
        RIMValue r;
        r.type = ValueType::kJobProgress;
        r.u.jobProgress = v;
        return r;
    }

    static RIMValue FaultCode(std::uint32_t v)
    {
        RIMValue r;
        r.type = ValueType::kFaultCode;
        r.u.faultCode = v;
        return r;
    }
};

} // namespace rim
