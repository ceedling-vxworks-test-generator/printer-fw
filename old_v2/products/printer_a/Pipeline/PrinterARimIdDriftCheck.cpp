//
// PrinterARimIdDriftCheck.cpp - C ミラーヘッダと C++ 側定義のずれをビルド時に検出する。
//
// このファイル自体は何も実行しない(static_assert のみ)。C 側の rim_data_ids.h /
// rim_capability_ids.h / rim_capability_types.h を直接 C++ から include し、
// 対応する C++ 側の値・サイズと突き合わせる。
//
// ずれると「C から投入した id が想定と違う機種データを指してしまう」
// 「Capability のバイト列を C 側が誤ったサイズで読んでしまう」といった、
// 実行時まで気付けない不具合になるため、ビルド時に落とす。
//

#include "CapabilityItem/PrinterACapabilityIds.hpp"
#include "CapabilityItem/PrinterACapabilityTypes.hpp"
#include "CapabilityItem/rim_capability_ids.h"
#include "CapabilityItem/rim_capability_types.h"
#include "DataItem/RIMDataId.hpp"
#include "DataItem/rim_data_ids.h"

namespace
{

// --- データ id ---

static_assert(
    static_cast<int>(rim::RIMDataId::kTemperatureSensorA) == RIM_ID_TEMPERATURE_SENSOR_A,
    "rim_data_ids.h と rim::RIMDataId がずれている");
static_assert(
    static_cast<int>(rim::RIMDataId::kTemperatureSensorB) == RIM_ID_TEMPERATURE_SENSOR_B,
    "rim_data_ids.h と rim::RIMDataId がずれている");
static_assert(
    static_cast<int>(rim::RIMDataId::kHumiditySensor) == RIM_ID_HUMIDITY_SENSOR,
    "rim_data_ids.h と rim::RIMDataId がずれている");
static_assert(
    static_cast<int>(rim::RIMDataId::kUpperDoorOpen) == RIM_ID_UPPER_DOOR_OPEN,
    "rim_data_ids.h と rim::RIMDataId がずれている");
static_assert(
    static_cast<int>(rim::RIMDataId::kRightDoorOpen) == RIM_ID_RIGHT_DOOR_OPEN,
    "rim_data_ids.h と rim::RIMDataId がずれている");
static_assert(
    static_cast<int>(rim::RIMDataId::kLeftDoorOpen) == RIM_ID_LEFT_DOOR_OPEN,
    "rim_data_ids.h と rim::RIMDataId がずれている");
static_assert(
    static_cast<int>(rim::RIMDataId::kStapleLevel) == RIM_ID_STAPLE_LEVEL,
    "rim_data_ids.h と rim::RIMDataId がずれている");
static_assert(
    static_cast<int>(rim::RIMDataId::kTonerLevel) == RIM_ID_TONER_LEVEL,
    "rim_data_ids.h と rim::RIMDataId がずれている");
static_assert(
    static_cast<int>(rim::RIMDataId::kJobActive) == RIM_ID_JOB_ACTIVE,
    "rim_data_ids.h と rim::RIMDataId がずれている");
static_assert(
    static_cast<int>(rim::RIMDataId::kJobId) == RIM_ID_JOB_ID,
    "rim_data_ids.h と rim::RIMDataId がずれている");
static_assert(
    rim::kRIMDataIdCount == RIM_ID_COUNT,
    "rim_data_ids.h と rim::RIMDataId がずれている(件数)");

// --- Capability id ---

static_assert(
    static_cast<int>(rim::kCapEnvironment) == RIM_CAP_ENVIRONMENT,
    "rim_capability_ids.h と rim::kCapEnvironment がずれている");
static_assert(
    static_cast<int>(rim::kCapError) == RIM_CAP_ERROR,
    "rim_capability_ids.h と rim::kCapError がずれている");
static_assert(
    static_cast<int>(rim::kCapPrintReady) == RIM_CAP_PRINT_READY,
    "rim_capability_ids.h と rim::kCapPrintReady がずれている");
static_assert(
    static_cast<int>(rim::kCapConsumable) == RIM_CAP_CONSUMABLE,
    "rim_capability_ids.h と rim::kCapConsumable がずれている");
static_assert(
    static_cast<int>(rim::kCapJob) == RIM_CAP_JOB,
    "rim_capability_ids.h と rim::kCapJob がずれている");
static_assert(
    static_cast<int>(rim::kPrinterACapabilityCount) == RIM_CAP_COUNT,
    "rim_capability_ids.h と rim::kPrinterACapabilityCount がずれている(件数)");

// --- Capability payload のバイトレイアウト ---
//
// フィールドの型と並びを手で合わせただけなので、サイズが一致することだけは
// ここで機械的に確認できる(内容の一致は確認できない = 手を合わせる側の責任)。

static_assert(
    sizeof(rim_environment_capability_t) == sizeof(rim::EnvironmentCapability),
    "rim_environment_capability_t が EnvironmentCapability とサイズ不一致");
static_assert(
    sizeof(rim_print_ready_capability_t) == sizeof(rim::PrintReadyCapability),
    "rim_print_ready_capability_t が PrintReadyCapability とサイズ不一致");
static_assert(
    sizeof(rim_consumable_capability_t) == sizeof(rim::ConsumableCapability),
    "rim_consumable_capability_t が ConsumableCapability とサイズ不一致");
static_assert(
    sizeof(rim_job_capability_t) == sizeof(rim::JobCapability),
    "rim_job_capability_t が JobCapability とサイズ不一致");
static_assert(
    sizeof(rim_error_capability_t) == sizeof(rim::ErrorCapability),
    "rim_error_capability_t が ErrorCapability とサイズ不一致");
static_assert(
    RIM_PRINTER_A_MAX_ERRORS == rim::kPrinterAMaxErrors,
    "RIM_PRINTER_A_MAX_ERRORS が kPrinterAMaxErrors とずれている");

} // namespace
