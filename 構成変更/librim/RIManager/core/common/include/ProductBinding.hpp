#pragma once

#include "ErrorRepository.hpp"
#include "ICapabilityRuleProvider.hpp"

namespace rim
{

//
// ProductBinding - Core と Product をつなぐ唯一の接点。
//
// Core は「どの Product 向けにビルドされたか」を知らない。ここで宣言した関数の
// **定義は products/<機種>/ 側にあり、リンク時に解決される**。
// これにより core → products のインクルード依存を作らずに、Core が Product の
// Capability 規則を使える(RIMDataId.hpp のヘッダ注入と同じ考え方)。
//
// 機種を差し替えるときは、リンクする product ライブラリを変えるだけでよい。
//

// Core が Product へ提供するサービス。
// Product の規則が Snapshot 以外の入力を必要とする場合(異常一覧など)に使う。
struct ProductServices
{
    const ErrorRepository& errorRepository;
};

//
// 以下2つは **Product が定義すること**(定義が無いとリンクエラーになる)。
//

// この Product の Capability 規則一式を生成する。
// 生成失敗(枠不足など)なら nullptr。動的確保はせず、Product 側の静的プールから
// 払い出す実装であること。
ICapabilityRuleProvider* CreateProductCapabilityRuleProvider(
    const ProductServices& services);

// 上で払い出したものを返却する。nullptr を渡してもよい(何もしない)。
void DestroyProductCapabilityRuleProvider(
    ICapabilityRuleProvider* provider);

} // namespace rim
