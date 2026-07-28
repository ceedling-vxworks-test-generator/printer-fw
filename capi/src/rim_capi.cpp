#include "rim_capi.h"

#include "core/RIMSystem.hpp"
#include "adapter/PrinterADataProfile.hpp"
#include "capability/PrinterACapabilityBuilder.hpp"
#include "datastore/Config.hpp"

namespace
{

/* ============================================================
 * Cミラーの整数値照合(手書きの2重管理がズレたらビルドで検知する)
 * ============================================================ */
static_assert(static_cast<int>(RIM_ID_TEMPERATURE)      == static_cast<int>(rim::RIMDataId::kTemperature),      "rim_id_t drifted from RIMDataId");
static_assert(static_cast<int>(RIM_ID_HUMIDITY)         == static_cast<int>(rim::RIMDataId::kHumidity),         "rim_id_t drifted from RIMDataId");
static_assert(static_cast<int>(RIM_ID_PRESSURE)         == static_cast<int>(rim::RIMDataId::kPressure),         "rim_id_t drifted from RIMDataId");
static_assert(static_cast<int>(RIM_ID_INK_LEVEL)        == static_cast<int>(rim::RIMDataId::kInkLevel),         "rim_id_t drifted from RIMDataId");
static_assert(static_cast<int>(RIM_ID_WIPER_LEVEL)      == static_cast<int>(rim::RIMDataId::kWiperLevel),       "rim_id_t drifted from RIMDataId");
static_assert(static_cast<int>(RIM_ID_COVER_OPEN)       == static_cast<int>(rim::RIMDataId::kCoverOpen),        "rim_id_t drifted from RIMDataId");
static_assert(static_cast<int>(RIM_ID_ESTOP)            == static_cast<int>(rim::RIMDataId::kEStop),            "rim_id_t drifted from RIMDataId");
static_assert(static_cast<int>(RIM_ID_JOB_PROGRESS)     == static_cast<int>(rim::RIMDataId::kJobProgress),      "rim_id_t drifted from RIMDataId");
static_assert(static_cast<int>(RIM_ID_FAULT_CODE)       == static_cast<int>(rim::RIMDataId::kFaultCode),        "rim_id_t drifted from RIMDataId");
static_assert(static_cast<int>(RIM_ID_MAINTENANCE_COUNT)== static_cast<int>(rim::RIMDataId::kMaintenanceCount), "rim_id_t drifted from RIMDataId");
static_assert(static_cast<int>(RIM_ID_UNIT_ALIVE)       == static_cast<int>(rim::RIMDataId::kUnitAlive),        "rim_id_t drifted from RIMDataId");
static_assert(static_cast<int>(RIM_ID_COUNT)            == static_cast<int>(rim::RIMDataId::kCount),            "rim_id_t drifted from RIMDataId");

static_assert(static_cast<int>(RIM_OK)               == static_cast<int>(rim::RIMResult::kOk),              "rim_result_t drifted from RIMResult");
static_assert(static_cast<int>(RIM_ERR_CONVERT)      == static_cast<int>(rim::RIMResult::kErrConvert),      "rim_result_t drifted from RIMResult");
static_assert(static_cast<int>(RIM_ERR_CLASSIFY)     == static_cast<int>(rim::RIMResult::kErrClassify),     "rim_result_t drifted from RIMResult");
static_assert(static_cast<int>(RIM_ERR_POST)         == static_cast<int>(rim::RIMResult::kErrPost),         "rim_result_t drifted from RIMResult");
static_assert(static_cast<int>(RIM_ERR_KIND_MISMATCH)== static_cast<int>(rim::RIMResult::kErrKindMismatch), "rim_result_t drifted from RIMResult");
static_assert(static_cast<int>(RIM_ERR_INVALID_ARG)  == static_cast<int>(rim::RIMResult::kErrInvalidArg),   "rim_result_t drifted from RIMResult");

static_assert(static_cast<int>(RIM_FAULT_NONE)           == static_cast<int>(rim::FaultState::kNone),          "rim_fault_state_t drifted from FaultState");
static_assert(static_cast<int>(RIM_FAULT_RAISED)         == static_cast<int>(rim::FaultState::kRaised),        "rim_fault_state_t drifted from FaultState");
static_assert(static_cast<int>(RIM_FAULT_CLEARED)        == static_cast<int>(rim::FaultState::kCleared),       "rim_fault_state_t drifted from FaultState");
static_assert(static_cast<int>(RIM_FAULT_ALL_CLEARED)    == static_cast<int>(rim::FaultState::kAllCleared),    "rim_fault_state_t drifted from FaultState");
static_assert(static_cast<int>(RIM_FAULT_UPDATED_HEAL)   == static_cast<int>(rim::FaultState::kUpdatedHeal),   "rim_fault_state_t drifted from FaultState");
static_assert(static_cast<int>(RIM_FAULT_UPDATED_ACTIVE) == static_cast<int>(rim::FaultState::kUpdatedActive), "rim_fault_state_t drifted from FaultState");

static_assert(static_cast<int>(RIM_PRIORITY_LOW)      == static_cast<int>(rim::CapabilityPriority::kLow),      "rim_capability_priority_t drifted");
static_assert(static_cast<int>(RIM_PRIORITY_NORMAL)   == static_cast<int>(rim::CapabilityPriority::kNormal),   "rim_capability_priority_t drifted");
static_assert(static_cast<int>(RIM_PRIORITY_HIGH)     == static_cast<int>(rim::CapabilityPriority::kHigh),     "rim_capability_priority_t drifted");
static_assert(static_cast<int>(RIM_PRIORITY_CRITICAL) == static_cast<int>(rim::CapabilityPriority::kCritical), "rim_capability_priority_t drifted");

static_assert(static_cast<int>(RIM_TRIGGER_ON_CHANGE) == static_cast<int>(rim::PublishTrigger::kOnChange), "rim_publish_trigger_t drifted");
static_assert(static_cast<int>(RIM_TRIGGER_PERIODIC)  == static_cast<int>(rim::PublishTrigger::kPeriodic), "rim_publish_trigger_t drifted");
static_assert(static_cast<int>(RIM_TRIGGER_THRESHOLD) == static_cast<int>(rim::PublishTrigger::kThreshold),"rim_publish_trigger_t drifted");
static_assert(static_cast<int>(RIM_TRIGGER_EVENT)     == static_cast<int>(rim::PublishTrigger::kEvent),    "rim_publish_trigger_t drifted");
static_assert(static_cast<int>(RIM_TRIGGER_INITIAL)   == static_cast<int>(rim::PublishTrigger::kInitial),  "rim_publish_trigger_t drifted");

static_assert(RIM_CAP_BIT_ERROR      == rim::KindBit(rim::CapabilityKind::kError),      "RIM_CAP_BIT_* drifted from CapabilityKind");
static_assert(RIM_CAP_BIT_JOB        == rim::KindBit(rim::CapabilityKind::kJob),        "RIM_CAP_BIT_* drifted from CapabilityKind");
static_assert(RIM_CAP_BIT_ENV        == rim::KindBit(rim::CapabilityKind::kEnv),        "RIM_CAP_BIT_* drifted from CapabilityKind");
static_assert(RIM_CAP_BIT_MAINT      == rim::KindBit(rim::CapabilityKind::kMaint),      "RIM_CAP_BIT_* drifted from CapabilityKind");
static_assert(RIM_CAP_BIT_HEALTH     == rim::KindBit(rim::CapabilityKind::kHealth),     "RIM_CAP_BIT_* drifted from CapabilityKind");
static_assert(RIM_CAP_BIT_SAFETY     == rim::KindBit(rim::CapabilityKind::kSafety),     "RIM_CAP_BIT_* drifted from CapabilityKind");
static_assert(RIM_CAP_BIT_CONSUMABLE == rim::KindBit(rim::CapabilityKind::kConsumable), "RIM_CAP_BIT_* drifted from CapabilityKind");
static_assert(RIM_CAP_BIT_PRINT      == rim::KindBit(rim::CapabilityKind::kPrint),      "RIM_CAP_BIT_* drifted from CapabilityKind");

static_assert(RIM_DOMAIN_NONE        == static_cast<uint32_t>(rim::kDomainNone),        "RIM_DOMAIN_* drifted from RegistryDomain");
static_assert(RIM_DOMAIN_FAULT       == static_cast<uint32_t>(rim::kDomainFault),       "RIM_DOMAIN_* drifted from RegistryDomain");
static_assert(RIM_DOMAIN_OPERATION   == static_cast<uint32_t>(rim::kDomainOperation),   "RIM_DOMAIN_* drifted from RegistryDomain");
static_assert(RIM_DOMAIN_ENVIRONMENT == static_cast<uint32_t>(rim::kDomainEnvironment), "RIM_DOMAIN_* drifted from RegistryDomain");
static_assert(RIM_DOMAIN_CONSUMABLE  == static_cast<uint32_t>(rim::kDomainConsumable),  "RIM_DOMAIN_* drifted from RegistryDomain");
static_assert(RIM_DOMAIN_SAFETY      == static_cast<uint32_t>(rim::kDomainSafety),      "RIM_DOMAIN_* drifted from RegistryDomain");
static_assert(RIM_DOMAIN_MAINTENANCE == static_cast<uint32_t>(rim::kDomainMaintenance), "RIM_DOMAIN_* drifted from RegistryDomain");
static_assert(RIM_DOMAIN_HEALTH      == static_cast<uint32_t>(rim::kDomainHealth),      "RIM_DOMAIN_* drifted from RegistryDomain");
static_assert(RIM_DOMAIN_CURRENT_ALL == static_cast<uint32_t>(rim::kDomainCurrentAll),  "RIM_DOMAIN_* drifted from RegistryDomain");

static_assert(RIM_FAULT_CAP == rim::kFaultCap, "RIM_FAULT_CAP drifted from rim::kFaultCap");
static_assert(RIM_SUBSCRIBER_MAX == rim::SubscriptionBroker::kMaxSubscribers, "RIM_SUBSCRIBER_MAX drifted from SubscriptionBroker::kMaxSubscribers");

/* ============================================================
 * 型変換ヘルパ(C++ -> C)
 * ============================================================ */
rim_capability_set_t ToC(const rim::CapabilitySet& s)
{
    rim_capability_set_t c{};
    if (s.error)  { c.has_error = true;  c.err_has_error    = s.error->hasError;   c.err_active_count = s.error->activeCount; }
    if (s.job)    { c.has_job = true;    c.job_active       = s.job->active;       c.job_progress     = s.job->progress; }
    if (s.env)    { c.has_env = true;    c.env_in_range     = s.env->inRange; }
    if (s.maint)  { c.has_maint = true;  c.maint_needed     = s.maint->needed; }
    if (s.health) { c.has_health = true; c.health_healthy   = s.health->healthy; }
    if (s.safety) { c.has_safety = true; c.safety_safe      = s.safety->safe; }
    if (s.consumable) { c.has_consumable = true; c.cons_ink_low = s.consumable->inkLow; c.cons_wiper_low = s.consumable->wiperLow; }
    if (s.print)  { c.has_print = true;  c.print_printable  = s.print->printable; }
    return c;
}

rim_machine_capability_t ToC(const rim::MachineCapability& mc)
{
    rim_machine_capability_t c{};
    c.capabilities  = ToC(mc.capabilities);
    c.priority      = static_cast<rim_capability_priority_t>(mc.priority);
    c.changed_kinds = mc.changedKinds;
    c.trigger       = static_cast<rim_publish_trigger_t>(mc.trigger);
    return c;
}

/* ISubscriber(C++抽象クラス) を Cの関数ポインタ+ctxへ橋渡しする。 */
class CallbackSubscriber final : public rim::ISubscriber
{
public:
    bool              used{false};
    rim_capability_cb cb{nullptr};
    void*             ctx{nullptr};

    void OnPublish(const rim::MachineCapability& mc) override
    {
        if (!cb) return;
        rim_machine_capability_t c = ToC(mc);
        cb(&c, ctx);
    }
};

CallbackSubscriber g_subscribers[RIM_SUBSCRIBER_MAX];

/* 機種の結線(PrinterA固定)。将来別機種を扱うなら、この1点だけが差し替え対象になる。 */
rim::PrinterADataProfile       g_profile;
rim::PrinterACapabilityBuilder g_builder;
rim::RIMSystem                 g_system{g_profile, g_profile, g_builder};

} // namespace

/* ============================================================
 * ライフサイクル / 受理 / 駆動
 * ============================================================ */
rim_result_t rim_init(void)
{
    return static_cast<rim_result_t>(g_system.Init());
}

void rim_shutdown(void)
{
    g_system.Shutdown();
}

void rim_dispatch(void)
{
    g_system.Dispatch();
}

rim_raw_value_t rim_raw_scalar(double v)
{
    rim_raw_value_t r{};
    r.kind     = RIM_RAW_SCALAR;
    r.as.scalar = v;
    return r;
}

rim_raw_value_t rim_raw_struct(const void* p, size_t elem_size)
{
    rim_raw_value_t r{};
    r.kind = RIM_RAW_BYTES;
    r.as.bytes.data  = p;
    r.as.bytes.size  = elem_size;
    r.as.bytes.count = 1;
    return r;
}

rim_raw_value_t rim_raw_array(const void* p, size_t elem_size, size_t count)
{
    rim_raw_value_t r{};
    r.kind = RIM_RAW_BYTES;
    r.as.bytes.data  = p;
    r.as.bytes.size  = elem_size * count;
    r.as.bytes.count = count;
    return r;
}

rim_result_t rim_push(rim_id_t id, rim_raw_value_t raw, const rim_context_t* ctx)
{
    rim::RawValue rv;
    if (raw.kind == RIM_RAW_SCALAR) {
        rv = rim::RawValue::Scalar(raw.as.scalar);
    } else {
        rv.kind  = rim::RawValue::Kind::kBytes;
        rv.data  = raw.as.bytes.data;
        rv.size  = raw.as.bytes.size;
        rv.count = raw.as.bytes.count;
    }

    rim::DataContext cxx_ctx;
    if (ctx) {
        if (ctx->has_fault_state) cxx_ctx.faultState = static_cast<rim::FaultState>(ctx->fault_state);
        if (ctx->has_scale_x1000) cxx_ctx.scaleX1000 = ctx->scale_x1000;
        if (ctx->has_key)         cxx_ctx.key        = ctx->key;
    }

    rim::RIMResult r = g_system.Adapter().Push(
        static_cast<rim::RIMDataId>(id), rv, ctx ? &cxx_ctx : nullptr);
    return static_cast<rim_result_t>(r);
}

/* ============================================================
 * Capability(通知)
 * ============================================================ */
rim_subscription_t rim_subscribe(rim_capability_cb cb, void* ctx, uint32_t interested_kinds)
{
    if (!cb) return -1;

    for (int i = 0; i < RIM_SUBSCRIBER_MAX; ++i) {
        if (!g_subscribers[i].used) {
            g_subscribers[i].cb  = cb;
            g_subscribers[i].ctx = ctx;

            rim::SubscriptionSet set;
            set.interested = interested_kinds;
            if (g_system.Publisher().Subscribe(&g_subscribers[i], set) != rim::RIMResult::kOk) {
                g_subscribers[i].cb = nullptr;
                g_subscribers[i].ctx = nullptr;
                return -1;
            }
            g_subscribers[i].used = true;
            return static_cast<rim_subscription_t>(i);
        }
    }
    return -1; /* 空きスロットなし */
}

void rim_unsubscribe(rim_subscription_t handle)
{
    if (handle < 0 || handle >= RIM_SUBSCRIBER_MAX) return;
    if (!g_subscribers[handle].used) return;

    g_system.Publisher().Unsubscribe(&g_subscribers[handle]);
    g_subscribers[handle].used = false;
    g_subscribers[handle].cb   = nullptr;
    g_subscribers[handle].ctx  = nullptr;
}

/* ============================================================
 * Accessor(参照専用・Pull)
 * ============================================================ */
rim_printer_status_t rim_get_status(uint32_t domains, bool include_capability)
{
    rim_printer_status_t out{};

    rim::PrinterStatusRequest req;
    req.domains           = domains;
    req.includeCapability = include_capability;

    const rim::PrinterStatus st = g_system.Accessor().GetPrinterStatus(req);

    if (st.data) {
        out.has_data = true;
        rim_machine_snapshot_t& d = out.data;

        if (st.data->fault) {
            d.fault_present     = true;
            d.fault_active_count = st.data->fault->activeCount;
            d.fault_healed_count = st.data->fault->healedCount;
            for (std::size_t i = 0; i < RIM_FAULT_CAP; ++i) {
                d.fault_active_codes[i] = st.data->fault->activeCodes[i];
            }
        }
        if (st.data->operation) {
            d.op_present      = true;
            d.op_job_present  = st.data->operation->jobPresent;
            d.op_job_progress = st.data->operation->jobProgress;
            d.op_job_active   = st.data->operation->jobActive;
        }
        if (st.data->currentValue) {
            d.cur_present = true;
            const auto& cv = *st.data->currentValue;

            if (cv.environment) {
                d.env_present = true;
                if (cv.environment->temperatureX100) { d.has_temperature_x100 = true; d.temperature_x100 = *cv.environment->temperatureX100; }
                if (cv.environment->humidity)         { d.has_humidity         = true; d.humidity         = *cv.environment->humidity; }
                if (cv.environment->pressure)         { d.has_pressure         = true; d.pressure         = *cv.environment->pressure; }
            }
            if (cv.consumable) {
                d.cons_present = true;
                if (cv.consumable->inkLevel)   { d.has_ink_level   = true; d.ink_level   = *cv.consumable->inkLevel; }
                if (cv.consumable->wiperLevel) { d.has_wiper_level = true; d.wiper_level = *cv.consumable->wiperLevel; }
            }
            if (cv.safety) {
                d.safety_present = true;
                if (cv.safety->coverOpen) { d.has_cover_open = true; d.cover_open = *cv.safety->coverOpen; }
                if (cv.safety->eStop)     { d.has_e_stop     = true; d.e_stop     = *cv.safety->eStop; }
            }
            if (cv.maintenance) {
                d.maint_present = true;
                if (cv.maintenance->counter) { d.has_maint_counter = true; d.maint_counter = *cv.maintenance->counter; }
            }
            if (cv.health) {
                d.health_present = true;
                if (cv.health->unitAlive) { d.has_unit_alive = true; d.unit_alive = *cv.health->unitAlive; }
            }
        }
    }

    if (st.capability) {
        out.has_capability = true;
        out.capability      = ToC(*st.capability);
    }

    return out;
}
