#pragma once

#include "CapabilityId.hpp"

namespace rim
{

//
// PrinterA が定義する Capability の識別子。
//
// CapabilityId の値割り当ては **Product の責務**である。Core は整数として扱うだけで、
// ここに並ぶ名前を一切知らない(Core は製品知識を持たない)。
//
// Core が配列添字として使うため 0 から連番で割り当て、kPrinterACapabilityCount 以下に
// 収めること(上限は core の kCapabilityMaxCount)。
//
enum : CapabilityId
{
    kCapEnvironment = 0,
    kCapError,
    kCapPrintReady,
    kCapConsumable,
    kCapJob,

    kPrinterACapabilityCount
};

} // namespace rim
