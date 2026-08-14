#pragma once

#include <cstdint>

namespace rim
{

//
// CapabilityId - Capability の識別子。
//
// 値の割り当ては **Product** が行い、Core は整数として扱うだけで意味を知らない。
// Core が "Environment" や "PrintReady" といった具体名を型として持たないための要。
// (方針: Core は製品知識を持たない / Core は Product を参照してはならない)
//
// Product 側は連番で割り当てること(Core が配列添字として使えるようにするため)。
// 例) products/printer_a/CapabilityItem/PrinterACapabilities.hpp
//
using CapabilityId = std::uint16_t;

inline constexpr CapabilityId kInvalidCapabilityId =
    static_cast<CapabilityId>(0xFFFFu);

} // namespace rim
