#pragma once

namespace rim
{

enum class ValueType
{
    kBool,
    kInt32,
    kUInt32,
    kInt64,
    kUInt64,
    kDouble,
    kString,

    kBinary,

    kStruct,
    kArray,
    kObject
};

} // namespace rim