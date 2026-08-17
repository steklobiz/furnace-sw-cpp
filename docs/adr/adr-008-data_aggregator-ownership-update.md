# ADR: DataAggregator Ownership and Update Responsibility

## Status

Accepted

## Context

`DataAggregator` provides application data to the UI. Currently, UI is its only consumer.

We want `Application` to own the aggregator while keeping UI responsible for preparing and consuming its own data snapshot.

## Decision

- `Application` owns `DataAggregator`.
- `UI` receives a reference to `DataAggregator` during `init()`.
- `UI::process()` calls `DataAggregator::update()`.
- `DataAggregator` accesses its application data sources through references provided during initialization.
- `DataAggregator` knows nothing about UI, pages, or renderers.
- No separate scheduler task is created for `DataAggregator`.

```text
Application owns
        │
        ▼
DataAggregator
        ▲
        │ update()
       UI
       
```
## Rationale

UI is currently the only consumer of aggregated data, so updating the snapshot when UI processes is simple and avoids an unnecessary scheduler task.

Ownership remains with Application, preserving the separation between application data and presentation.

## Consequences
DataAggregator is updated at the UI processing rate.
UI always works with a freshly updated snapshot.
If another independent consumer appears later, the update responsibility can be reconsidered.       