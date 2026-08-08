# DataAccessor Layer

## 1.Capability取得

### シーケンス概要
Applicationからの要求に応じて最新Capabilityを取得するシーケンス。

```mermaid
sequenceDiagram
participant App as Application
participant Query as DataAccessor:CapabilityQueryService
participant Provider as Capability:IRIMCapabilityProvider

Note over Query,Provider: Layer Boundary (DataAccessor -> Capability)

App->>Query: getCapability(Category)
Query->>Provider: getCapability(Category)
Provider-->>Query: Capability
Query-->>App: Capability
```

## 2.Condition評価
### シーケンス概要
Capabilityを利用して業務条件を評価し、判定結果を返却するシーケンス。

```mermaid
sequenceDiagram
participant App as Application
participant Eval as DataAccessor:ConditionEvaluator
participant Query as DataAccessor:CapabilityQueryService

App->>Eval: evaluate(Condition)
Eval->>Query: getCapability()
Query-->>Eval: Capability
Eval->>Eval: evaluateRule()
Eval-->>App: ConditionResult
```

## 3.Registry取得

### シーケンス概要
DataStoreが保持するData情報をRIMSnapshotとして取得するシーケンス。

```mermaid
sequenceDiagram
participant App as Application
participant Query as DataAccessor:RegistryQueryService
participant Reader as DataStore:AggregateSnapshotReader

Note over Query,Reader: Layer Boundary (DataAccessor -> DataStore)

App->>Query: getRegistry(domain)
Query->>Reader: capture(request)
Reader-->>Query: RIMSnapshot
Query-->>App: RIMSnapshot
```
---