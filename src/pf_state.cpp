/**
 * @file pf_state.cpp
 * @brief ② Abstract State Layer（評価器レジストリ）の実装。
 *
 * state_registry クラスが評価器・確定値を保持する単一インスタンス（シングルトン）。
 * 評価器（純関数）を登録し、要求に応じて実行する。確定値は monitor_engine が書き戻す。
 */
#include "printer_fw/pf_state.h"
#include "printer_fw/pf_config.h"
#include "pf_priv.hpp"

namespace {

class state_registry {
public:
    pf_result_t init(const pf_state_desc_t* desc, size_t count);
    pf_result_t eval(pf_state_id_t id, int32_t* out) const;
    pf_result_t get(pf_state_id_t id, int32_t* out) const;
    void        store(pf_state_id_t id, int32_t value);

    size_t        count() const { return count_; }
    pf_state_id_t id_at(size_t slot) const { return (slot < count_) ? desc_[slot].id : static_cast<pf_state_id_t>(0xFFFFu); }
    int32_t       initial_at(size_t slot) const { return (slot < count_) ? desc_[slot].initial : 0; }

private:
    int index_of(pf_state_id_t id) const;

    bool            init_  = false;
    size_t          count_ = 0;
    pf_state_desc_t desc_[PF_STATE_MAX]{};
    int32_t         last_[PF_STATE_MAX]{};
};

int state_registry::index_of(pf_state_id_t id) const
{
    if (static_cast<size_t>(id) < count_ && desc_[id].id == id) return static_cast<int>(id);
    for (size_t i = 0; i < count_; ++i) {
        if (desc_[i].id == id) return static_cast<int>(i);
    }
    return -1;
}

pf_result_t state_registry::init(const pf_state_desc_t* desc, size_t count)
{
    if (!desc) return PF_ERR_INVALID_ARG;
    if (count > PF_STATE_MAX) return PF_ERR_NO_SPACE;

    for (size_t i = 0; i < count; ++i) {
        if (desc[i].id == static_cast<pf_state_id_t>(0xFFFFu) || desc[i].eval == nullptr) return PF_ERR_INVALID_ARG;
        for (size_t j = 0; j < i; ++j) {
            if (desc[j].id == desc[i].id) return PF_ERR_INVALID_ARG;
        }
        desc_[i] = desc[i];
        last_[i] = desc[i].initial;
    }
    count_ = count;
    init_  = true;
    return PF_OK;
}

pf_result_t state_registry::eval(pf_state_id_t id, int32_t* out) const
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (!out) return PF_ERR_INVALID_ARG;
    int s = index_of(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    return desc_[s].eval(out, desc_[s].ctx);
}

pf_result_t state_registry::get(pf_state_id_t id, int32_t* out) const
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (!out) return PF_ERR_INVALID_ARG;
    int s = index_of(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    *out = last_[s];
    return PF_OK;
}

void state_registry::store(pf_state_id_t id, int32_t value)
{
    int s = index_of(id);
    if (s >= 0) last_[s] = value;
}

state_registry g_state;

} // namespace

pf_result_t pf_state_init(const pf_state_desc_t* desc, size_t count) { return g_state.init(desc, count); }
pf_result_t pf_state_eval(pf_state_id_t id, int32_t* out)            { return g_state.eval(id, out); }
pf_result_t pf_state_get (pf_state_id_t id, int32_t* out)            { return g_state.get(id, out); }

/* --- 内部API（pf_priv.hpp）：monitor_engine から利用 --- */
size_t        pf_state__count()                          { return g_state.count(); }
pf_state_id_t pf_state__id_at(size_t slot)                { return g_state.id_at(slot); }
int32_t       pf_state__initial_at(size_t slot)           { return g_state.initial_at(slot); }
void          pf_state__store(pf_state_id_t id, int32_t value) { g_state.store(id, value); }
