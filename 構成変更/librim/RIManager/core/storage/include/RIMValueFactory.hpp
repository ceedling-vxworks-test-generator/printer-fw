#pragma once

#include "RIMValue.hpp"

namespace rim
{

namespace detail
{

template<typename TValue>
RIMValue CreateValue(
    ValueType type,
    TValue storageValue,
    TValue RIMValueStorage::* member)
{
    RIMValue result{};

    result.type = type;

    result.value.*member = storageValue;

    return result;
}

} // namespace detail

class RIMValueFactory
{
public:

    static RIMValue CreateBool(
        bool value)
    {
        return detail::CreateValue(
            ValueType::kBool,
            value,
            &RIMValueStorage::b);
    }

    static RIMValue CreateInt32(
        std::int32_t value)
    {
        return detail::CreateValue(
            ValueType::kInt32,
            value,
            &RIMValueStorage::i32);
    }

    static RIMValue CreateUInt32(
        std::uint32_t value)
    {
        return detail::CreateValue(
            ValueType::kUInt32,
            value,
            &RIMValueStorage::u32);
    }

    static RIMValue CreateInt64(
        std::int64_t value)
    {
        return detail::CreateValue(
            ValueType::kInt64,
            value,
            &RIMValueStorage::i64);
    }

    static RIMValue CreateUInt64(
        std::uint64_t value)
    {
        return detail::CreateValue(
            ValueType::kUInt64,
            value,
            &RIMValueStorage::u64);
    }

    static RIMValue CreateDouble(
        double value)
    {
        return detail::CreateValue(
            ValueType::kDouble,
            value,
            &RIMValueStorage::d);
    }

    static RIMValue CreateString(
        const char* value)
    {
        return detail::CreateValue(
            ValueType::kString,
            value,
            &RIMValueStorage::str);
    }
};

} // namespace rim