#pragma once

//
// DomainSet - 変化ドメインのビットマスク。rim_domain_set_t 相当。
//

#include <cstdint>

namespace rim
{

using DomainSet = std::uint32_t;

constexpr DomainSet kDomainNone      = 0u;
constexpr DomainSet kDomainFault     = 1u << 0;
constexpr DomainSet kDomainOperation = 1u << 1;
constexpr DomainSet kDomainCurrent   = 1u << 2;

} // namespace rim
