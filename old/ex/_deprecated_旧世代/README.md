# 廃止（旧②世代）— 参照非推奨

本フォルダのファイルは、**2026-07-03時点の旧設計世代（②）**であり、
**③最新基本設計（2026-07-07）を正とする方針決定（整合ギャップ分析_v1.md）により廃止**した。

## 廃止理由

③（現行）と構造が互換しないため。主な相違：

| 観点 | ②（本フォルダ・廃止） | ③（正） |
|------|---------------------|---------|
| 入力分類 | 単一Queue → 単一Dispatcher | 3レーン（Fault / Operation / CurrentValue） |
| Store構造 | 意味別 7 Store | 性質別 3 Registry + MachineRegistry集約 |
| Snapshot配送 | Push型（SnapshotTrigger → Full Snapshot） | Pull型（notifyUpdated → capture、部分Snapshot） |

## 対象ファイル
- DataStoreDispatcher詳細設計書.md
- DataStoreQueue詳細設計書.md
- DomainDataStore詳細設計書.md
- SnapshotTrigger詳細設計書.md
- SystemSnapshot _SnapshotProvider 詳細設計書.md
- 隠蔽_DataStoreLayer詳細設計書.md（旧・大部の詳細設計）

不要と判断した場合は本フォルダごと削除してよい。
