#pragma once

/*
 * rim_ids.h - 新機種テンプレ。データ種別識別子(Cミラー)。機種固有(devices側)。
 * TODO: datastore/RIMDataId.hpp(C++側)と同じ整数値・同じ並びで列挙する。
 * 整合性は capi/src/rim_capi.cpp の static_assert 群がビルド時に検証する。
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /* TODO: 例: RIM_ID_TEMPERATURE = 0, RIM_ID_HUMIDITY, ... */
    RIM_ID_COUNT = 0  /* TODO: 実データを足したら末尾へ移す(番兵) */
} rim_id_t;

#ifdef __cplusplus
}
#endif
