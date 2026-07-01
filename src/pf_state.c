/**
 * @file pf_state.c
 * @brief ② Abstract State Layer（評価器レジストリ）の実装スケルトン。
 *
 * 評価器（純関数）を登録し、要求に応じて実行する。確定値は Monitor が pf_state__store で書き戻す。
 */
#include "printer_fw/pf_state.h"
#include "printer_fw/pf_config.h"
#include "pf_priv.h"

static bool            g_init;
static size_t          g_count;
static pf_state_desc_t g_desc[PF_STATE_MAX];
static int32_t         g_last[PF_STATE_MAX];

static int idx(pf_state_id_t id)
{
    if ((size_t)id < g_count && g_desc[id].id == id) return (int)id;
    for (size_t i = 0; i < g_count; ++i) {
        if (g_desc[i].id == id) return (int)i;
    }
    return -1;
}

pf_result_t pf_state_init(const pf_state_desc_t* desc, size_t count)
{
    if (!desc) return PF_ERR_INVALID_ARG;
    if (count > PF_STATE_MAX) return PF_ERR_NO_SPACE;

    for (size_t i = 0; i < count; ++i) {
        if (desc[i].id == (pf_state_id_t)0xFFFFu || desc[i].eval == NULL) return PF_ERR_INVALID_ARG;
        for (size_t j = 0; j < i; ++j) {
            if (desc[j].id == desc[i].id) return PF_ERR_INVALID_ARG;
        }
        g_desc[i] = desc[i];
        g_last[i] = desc[i].initial;
    }
    g_count = count;
    g_init  = true;
    return PF_OK;
}

pf_result_t pf_state_eval(pf_state_id_t id, int32_t* out)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    if (!out) return PF_ERR_INVALID_ARG;
    int s = idx(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    return g_desc[s].eval(out, g_desc[s].ctx);
}

pf_result_t pf_state_get(pf_state_id_t id, int32_t* out)
{
    if (!g_init) return PF_ERR_NOT_INIT;
    if (!out) return PF_ERR_INVALID_ARG;
    int s = idx(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    *out = g_last[s];
    return PF_OK;
}

/* --- 内部API（pf_priv.h） --- */
size_t        pf_state__count(void)               { return g_count; }
pf_state_id_t pf_state__id_at(size_t slot)        { return (slot < g_count) ? g_desc[slot].id : (pf_state_id_t)0xFFFFu; }
int32_t       pf_state__initial_at(size_t slot)   { return (slot < g_count) ? g_desc[slot].initial : 0; }

void pf_state__store(pf_state_id_t id, int32_t value)
{
    int s = idx(id);
    if (s >= 0) g_last[s] = value;
}
