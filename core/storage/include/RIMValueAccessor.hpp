#pragma once

#include <cstdint>

#include "RIMValue.hpp"
#include "BinaryStoreValue.hpp"
#include "BinaryInput.hpp"

namespace rim
{

namespace detail
{

template<typename TValue>
bool GetValue(
    ValueType expectedType,
    const RIMValue& value,
    TValue RIMValueStorage::* member,
    TValue& out)
{
    if (value.type != expectedType)
    {
        return false;
    }

    out = value.value.*member;

    return true;
}

} // namespace detail

class RIMValueAccessor
{
public:

    static bool GetBool(
        const RIMValue& value,
        bool& out)
    {
        return detail::GetValue(
            ValueType::kBool,
            value,
            &RIMValueStorage::b,
            out);
    }

    static bool GetInt32(
        const RIMValue& value,
        std::int32_t& out)
    {
        return detail::GetValue(
            ValueType::kInt32,
            value,
            &RIMValueStorage::i32,
            out);
    }

    static bool GetUInt32(
        const RIMValue& value,
        std::uint32_t& out)
    {
        return detail::GetValue(
            ValueType::kUInt32,
            value,
            &RIMValueStorage::u32,
            out);
    }

    static bool GetInt64(
        const RIMValue& value,
        std::int64_t& out)
    {
        return detail::GetValue(
            ValueType::kInt64,
            value,
            &RIMValueStorage::i64,
            out);
    }

    static bool GetUInt64(
        const RIMValue& value,
        std::uint64_t& out)
    {
        return detail::GetValue(
            ValueType::kUInt64,
            value,
            &RIMValueStorage::u64,
            out);
    }

    static bool GetDouble(
        const RIMValue& value,
        double& out)
    {
        return detail::GetValue(
            ValueType::kDouble,
            value,
            &RIMValueStorage::d,
            out);
    }

    static bool GetString(
        const RIMValue& value,
        const char*& out)
    {
        return detail::GetValue(
            ValueType::kString,
            value,
            &RIMValueStorage::str,
            out);
    }

    static bool GetBinary(
        const RIMValue& value,
        const BinaryStoreValue*& binary)
    {
        if (value.type !=
            ValueType::kBinary)
        {
            return false;
        }

        binary =
            static_cast<
                const BinaryStoreValue*>(
                    value.value.ptr);

        return true;
    }

    static bool GetBinaryStore(
        const RIMValue& value,
        BinaryStoreValue*& binary)
    {
        if (value.type != ValueType::kBinary)
        {
            return false;
        }

        binary =
            static_cast<BinaryStoreValue*>(
                value.value.ptr);

        return true;
    }

    static bool GetBinaryInput(
        const RIMValue& value,
        BinaryInput*& out)
    {
        if (value.type != ValueType::kBinary)
        {
            return false;
        }

        out =
            static_cast<BinaryInput*>(
                value.value.ptr);

        return out != nullptr;
    }


};

} // namespace rim
