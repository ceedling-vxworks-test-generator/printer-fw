#pragma once

#include <cstring>
#include <memory>

#include "BinaryInput.hpp"
#include "BinaryStoreValue.hpp"
#include "RIMValue.hpp"
#include "RIMValueAccessor.hpp"
#include "RIMValueFactory.hpp"

namespace rim
{

inline RIMValue IdentityStore(
    const RIMValue&,
    const RIMValue& setValue,
    const void*)
{
    return setValue;
}

inline std::unique_ptr<BinaryStoreValue>
CreateBinaryStoreValue(
    const BinaryInput& input)
{
    auto result =
        std::make_unique<BinaryStoreValue>();

    result->data.resize(
        input.size);

    std::memcpy(
        result->data.data(),
        input.data,
        input.size);

    return result;
}

inline RIMValue StoreBinary(
    const RIMValue&,
    const RIMValue& setValue,
    const void*)
{
    BinaryInput* source{};

    if (!RIMValueAccessor::GetBinaryInput(
            setValue,
            source))
    {
        return {};
    }

    auto stored =
        CreateBinaryStoreValue(
            *source);

    return
        RIMValueFactory::CreateBinary(
            stored.release());
}

} // namespace rim