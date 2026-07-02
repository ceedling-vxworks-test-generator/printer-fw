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

/**
 * @brief 抽象状態の評価器レジストリ本体。
 *
 * 各状態IDに対して「評価器（純関数）」「直近の確定値」を保持する。
 * 評価そのものはこのクラスの責務ではなく、登録された評価器を呼び出すだけ
 * （実際の変化検知・確定は monitor_engine が行う）。
 */
class state_registry {
public:
    /** @brief 記述子配列で評価器レジストリを初期化する。@return init結果 */
    pf_result_t init(const pf_state_desc_t* desc, size_t count);
    /** @brief 登録済みの評価器をその場で実行し、現在の状態値を得る。@return eval結果 */
    pf_result_t eval(pf_state_id_t id, int32_t* out) const;
    /** @brief 直近に monitor_engine が確定した値を返す（評価器は実行しない）。@return get結果 */
    pf_result_t get(pf_state_id_t id, int32_t* out) const;
    /** @brief 確定値を書き戻す（monitor_engine 専用。id不正時は何もしない）。 */
    void        store(pf_state_id_t id, int32_t value);

    /** @brief 登録済みの状態数を返す（monitor_engine の走査用）。 */
    size_t        count() const { return count_; }
    /** @brief slot番目の状態IDを返す（範囲外なら予約ID 0xFFFF）。 */
    pf_state_id_t id_at(size_t slot) const { return (slot < count_) ? desc_[slot].id : static_cast<pf_state_id_t>(0xFFFFu); }
    /** @brief slot番目の状態の初期値を返す（範囲外なら0）。 */
    int32_t       initial_at(size_t slot) const { return (slot < count_) ? desc_[slot].initial : 0; }

private:
    /** @brief id からスロット番号を引く（稠密IDならO(1)、それ以外は線形探索）。@return スロット番号、未検出は-1 */
    int index_of(pf_state_id_t id) const;

    bool            init_  = false;             ///< init() 完了フラグ
    size_t          count_ = 0;                 ///< 登録済み状態数
    pf_state_desc_t desc_[PF_STATE_MAX]{};       ///< 状態ID・評価器・初期値などの記述子表
    int32_t         last_[PF_STATE_MAX]{};       ///< 直近にmonitorが確定した状態値
};

/**
 * @brief id をスロット番号へ変換する（pf_data の index_of と同じ稠密ID規約）。
 *
 * 内部処理: まず id 自身を直接添字として試し（O(1)）、外れたら記述子表を
 * 先頭から線形に探す（疎なID構成向けフォールバック）。
 */
int state_registry::index_of(pf_state_id_t id) const
{
    if (static_cast<size_t>(id) < count_ && desc_[id].id == id) return static_cast<int>(id);
    for (size_t i = 0; i < count_; ++i) {
        if (desc_[i].id == id) return static_cast<int>(i);
    }
    return -1;
}

/**
 * @brief 記述子配列でレジストリを初期化する。
 *
 * 内部処理: 予約ID(0xFFFF)や評価器未設定(eval==nullptr)を弾き、ID重複も検証したうえで
 * 記述子をコピーし、last_[] を各状態の初期値で埋める。
 */
pf_result_t state_registry::init(const pf_state_desc_t* desc, size_t count)
{
    if (!desc) return PF_ERR_INVALID_ARG;
    if (count > PF_STATE_MAX) return PF_ERR_NO_SPACE;

    for (size_t i = 0; i < count; ++i) {
        // 予約ID・評価器未設定は不正登録として弾く
        if (desc[i].id == static_cast<pf_state_id_t>(0xFFFFu) || desc[i].eval == nullptr) return PF_ERR_INVALID_ARG;
        // 重複IDチェック
        for (size_t j = 0; j < i; ++j) {
            if (desc[j].id == desc[i].id) return PF_ERR_INVALID_ARG;
        }
        desc_[i] = desc[i];
        last_[i] = desc[i].initial;   // 初期値をそのまま「直近の確定値」として扱う
    }
    count_ = count;
    init_  = true;
    return PF_OK;
}

/**
 * @brief 評価器をその場で実行する。
 *
 * 内部処理: スロットを特定し、記述子に登録された評価器(eval)を、記述子が
 * 持つ文脈(ctx)付きで呼び出すだけ（副作用のない純関数呼び出し）。
 */
pf_result_t state_registry::eval(pf_state_id_t id, int32_t* out) const
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (!out) return PF_ERR_INVALID_ARG;
    int s = index_of(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    return desc_[s].eval(out, desc_[s].ctx);
}

/** @brief 直近に確定した値を last_[] からそのまま読み出す（評価器は実行しない）。 */
pf_result_t state_registry::get(pf_state_id_t id, int32_t* out) const
{
    if (!init_) return PF_ERR_NOT_INIT;
    if (!out) return PF_ERR_INVALID_ARG;
    int s = index_of(id);
    if (s < 0) return PF_ERR_NOT_FOUND;
    *out = last_[s];
    return PF_OK;
}

/** @brief 確定値を書き戻す。id が見つからない場合は静かに無視する（monitor側の内部整合を壊さないため）。 */
void state_registry::store(pf_state_id_t id, int32_t value)
{
    int s = index_of(id);
    if (s >= 0) last_[s] = value;
}

/** 評価器レジストリの唯一のインスタンス（シングルトン・静的記憶域）。 */
state_registry g_state;

} // namespace

// ============================================================
// 以下は公開API（extern "C"、詳細は include/printer_fw/pf_state.h 参照）。
// state_registry の同名メソッドへ委譲するだけの薄いラッパー。
// ============================================================
pf_result_t pf_state_init(const pf_state_desc_t* desc, size_t count) { return g_state.init(desc, count); }
pf_result_t pf_state_eval(pf_state_id_t id, int32_t* out)            { return g_state.eval(id, out); }
pf_result_t pf_state_get (pf_state_id_t id, int32_t* out)            { return g_state.get(id, out); }

// ------------------------------------------------------------
// 内部API（pf_priv.hpp 宣言）：monitor_engine（pf_monitor.cpp）専用の橋渡し関数。
// 状態を全走査して確定値を書き戻すために必要な最小限のアクセスだけを公開する。
// ------------------------------------------------------------
size_t        pf_state__count()                          { return g_state.count(); }
pf_state_id_t pf_state__id_at(size_t slot)                { return g_state.id_at(slot); }
int32_t       pf_state__initial_at(size_t slot)           { return g_state.initial_at(slot); }
void          pf_state__store(pf_state_id_t id, int32_t value) { g_state.store(id, value); }
