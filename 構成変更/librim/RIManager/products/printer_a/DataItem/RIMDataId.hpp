#pragma once

//
// RIMDataId - PrinterA のデータ種別識別子。**機種固有**(products 側)。
//
// core は "RIMDataId.hpp" というヘッダ名と、そこに RIMDataId 型 / ToIndex() /
// kRIMDataIdCount があることにだけ依存し、**列挙子の中身を一切知らない**。
// 実体は機種ごとに products/<機種>/DataItem/RIMDataId.hpp が提供し、ビルド時の
// インクルードパス解決で core 側の #include "RIMDataId.hpp" に届く
// (FreeRTOSConfig.h と同じヘッダ注入方式)。
//
// 新機種を追加するときは products/skeleton/ を雛形にすること。
//
// 連番かつ kCount 番兵を置くこと。ValueStore / ParameterStore が **添字**として
// 使うため、値に穴を空けたり順序を入れ替えたりしてはいけない。
//

namespace rim
{

enum class RIMDataId
{
    kTemperatureSensorA = 0,
    kTemperatureSensorB,

    kHumiditySensor,

    kUpperDoorOpen,
    kRightDoorOpen,
    kLeftDoorOpen,

    kStapleLevel,
    kTonerLevel,

    kJobActive,
    kJobId,

    kCount
};

inline constexpr int ToIndex(RIMDataId id)
{
    return static_cast<int>(id);
}

inline constexpr int kRIMDataIdCount =
    static_cast<int>(RIMDataId::kCount);

} // namespace rim
