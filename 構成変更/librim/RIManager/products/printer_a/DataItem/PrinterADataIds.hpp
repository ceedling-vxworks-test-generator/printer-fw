#pragma once

#include <cstdint>

#include "RIMDataId.hpp"

namespace rim
{

//
// C API へ渡すためのデータ ID。
//
// C API はデータ ID を uint16_t で受ける(Core は番号の意味を知らない)。
// C++ 側では RIMDataId で書けたほうが安全なので、変換をここに1本化しておく。
// 「番号の割り当てを知っているのは Product だけ」という原則を守るため、
// この変換を Core 側に置かないこと。
//
constexpr std::uint16_t ToDataId(RIMDataId id)
{
    return static_cast<std::uint16_t>(id);
}

} // namespace rim
