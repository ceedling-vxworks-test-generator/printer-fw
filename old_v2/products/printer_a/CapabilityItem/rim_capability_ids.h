#pragma once

/*
 * rim_capability_ids.h - PrinterA が定義する Capability の識別子(C ミラー)。
 *
 * C++ 側の rim::kCapEnvironment 等(PrinterACapabilityIds.hpp)と同じ整数値を
 * 維持すること。整合性は Pipeline/PrinterARimIdDriftCheck.cpp の static_assert
 * 群がビルド時に検証する。
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    RIM_CAP_ENVIRONMENT = 0,
    RIM_CAP_ERROR,
    RIM_CAP_PRINT_READY,
    RIM_CAP_CONSUMABLE,
    RIM_CAP_JOB,
    RIM_CAP_COUNT
} rim_capability_id_t;

#ifdef __cplusplus
}
#endif
