#pragma once

//
// RIMValue - 正規化後の標準値。C表現のタグ付きunion。
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
        bool          boolean;
        std::uint32_t count;
    } u{};

    static RIMValue CelsiusX100(std::int32_t v) { RIMValue r; r.type = ValueType::kCelsiusX100; r.u.celsiusX100 = v; return r; }
    static RIMValue Percent(std::uint8_t v)     { RIMValue r; r.type = ValueType::kPercent;     r.u.percent = v;     return r; }
    static RIMValue JobProgress(std::uint8_t v) { RIMValue r; r.type = ValueType::kJobProgress; r.u.jobProgress = v; return r; }
    static RIMValue FaultCode(std::uint32_t v)  { RIMValue r; r.type = ValueType::kFaultCode;   r.u.faultCode = v;   return r; }
    static RIMValue Bool(bool v)                { RIMValue r; r.type = ValueType::kBool;        r.u.boolean = v;     return r; }
    static RIMValue UInt32(std::uint32_t v)     { RIMValue r; r.type = ValueType::kUInt32;      r.u.count = v;       return r; }
};

} // namespace rim
