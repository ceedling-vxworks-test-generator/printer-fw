#pragma once

/*
 * rim_capability_types.h - Capability payload を C から解釈するための構造体。
 *
 * RIManager_Get(Current)Capability() は Capability の中身を「不透明なバイト列」
 * として返す(Core は中身の型を知らない)。ここに定義した構造体は、C++ 側の
 * products/printer_a/CapabilityItem/PrinterACapabilityTypes.hpp と
 * **バイト単位で同一のレイアウト**になるよう手で合わせたものである。
 *
 * 整合性(サイズ一致)は Pipeline/PrinterARimIdDriftCheck.cpp の static_assert が
 * ビルド時に検証する。フィールドの型・並びを変える場合は両方を同時に直すこと。
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double temperature;
    double humidity;
} rim_environment_capability_t;

typedef struct
{
    bool ready;
} rim_print_ready_capability_t;

typedef struct
{
    int32_t tonerLevel;
    int32_t stapleLevel;
} rim_consumable_capability_t;

typedef struct
{
    bool    jobActive;
    int32_t jobId;
} rim_job_capability_t;

#define RIM_PRINTER_A_MAX_ERRORS 16

typedef enum
{
    RIM_ERROR_STATE_ACTIVE = 0,
    RIM_ERROR_STATE_RECOVERED = 1
} rim_error_state_t;

typedef enum
{
    RIM_ERROR_SEVERITY_INFO = 0,
    RIM_ERROR_SEVERITY_WARNING = 1,
    RIM_ERROR_SEVERITY_ERROR = 2,
    RIM_ERROR_SEVERITY_FATAL = 3
} rim_error_severity_t;

typedef struct
{
    uint32_t          errorCode;
    rim_error_state_t state;
    rim_error_severity_t severity;
} rim_error_info_t;

typedef struct
{
    rim_error_info_t errors[RIM_PRINTER_A_MAX_ERRORS];
    uint16_t         count;
    bool             overflow;
} rim_error_capability_t;

#ifdef __cplusplus
}
#endif
