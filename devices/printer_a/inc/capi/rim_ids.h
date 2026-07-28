#pragma once

/*
 * rim_ids.h - PrinterA のデータ種別識別子(Cミラー)。機種固有(devices側)。
 * framework/C++側の rim::RIMDataId(devices/printer_a/inc/datastore/RIMDataId.hpp)と
 * 同じ整数値・同じ並びを維持すること。整合性は capi/src/rim_capi.cpp の static_assert 群が
 * ビルド時に検証する。
 *
 * 新機種を追加する際は、この一覧を機種ごとの devices/<機種>/inc/capi/rim_ids.h として
 * コピー・置換する(devices/_skeleton/inc/capi/rim_ids.h を雛形にする)。
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RIM_ID_TEMPERATURE = 0,
    RIM_ID_HUMIDITY,
    RIM_ID_PRESSURE,
    RIM_ID_INK_LEVEL,
    RIM_ID_WIPER_LEVEL,
    RIM_ID_COVER_OPEN,
    RIM_ID_ESTOP,
    RIM_ID_JOB_PROGRESS,
    RIM_ID_FAULT_CODE,
    RIM_ID_MAINTENANCE_COUNT,
    RIM_ID_UNIT_ALIVE,
    RIM_ID_COUNT
} rim_id_t;

#ifdef __cplusplus
}
#endif
