#pragma once

#include <cstdint>

namespace rim
{

// 要求定義書はルーティング/QoS方針を規定していないため、
// 本製品では全DataItemを単一のルートにまとめている。
// 更新頻度に応じて個別ルートへ分割したい場合はここへ追加する。
enum RouteId : std::uint32_t
{
    ROUTE_DATA = 1,
};

}
