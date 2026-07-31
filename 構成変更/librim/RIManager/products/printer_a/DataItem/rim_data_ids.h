#pragma once

/*
 * rim_data_ids.h - PrinterA のデータ種別識別子(C ミラー)。機種固有。
 *
 * C++ 側の rim::RIMDataId(RIMDataId.hpp)と同じ整数値・同じ並びを維持すること。
 * 整合性は Pipeline/PrinterARimIdDriftCheck.cpp の static_assert 群がビルド時に
 * 検証する。
 *
 * 新機種を追加する際は、この一覧を products/<機種>/DataItem/rim_data_ids.h として
 * コピー・置換する(products/skeleton/DataItem/ を雛形にする)。
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    RIM_ID_TEMPERATURE_SENSOR_A = 0,
    RIM_ID_TEMPERATURE_SENSOR_B,
    RIM_ID_HUMIDITY_SENSOR,
    RIM_ID_UPPER_DOOR_OPEN,
    RIM_ID_RIGHT_DOOR_OPEN,
    RIM_ID_LEFT_DOOR_OPEN,
    RIM_ID_STAPLE_LEVEL,
    RIM_ID_TONER_LEVEL,
    RIM_ID_JOB_ACTIVE,
    RIM_ID_JOB_ID,
    RIM_ID_COUNT
} rim_data_id_t;

#ifdef __cplusplus
}
#endif
