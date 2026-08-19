ADR: DataAggregator Ownership and Update Responsibility
# Status

Accepted

# Context

DataAggregator provides application data to the UI. Currently, UI is its only consumer.

We want Application to own the aggregator while keeping it independent from UI and presentation.

# Decision
- Application owns DataAggregator.
- UI receives a reference to DataAggregator during init().
- Application data sources notify DataAggregator when relevant data or events are available.
- DataAggregator accesses its application data sources through references provided during initialization.
- DataAggregator knows nothing about UI, pages, or renderers.
- No separate scheduler task is created for DataAggregator.

```
Application owns
        │
        ▼
DataAggregator
        ▲
        │ notifications
        │
   Data Sources
```   
# Rationale

DataAggregator is updated when relevant data or events become available rather than at the UI processing rate.

This avoids unnecessary polling and keeps DataAggregator independent from UI timing.

Ownership remains with Application, preserving the separation between application data and presentation.

# Consequences

DataAggregator updates asynchronously through source notifications.

UI reads the current aggregated data when it processes or renders.

If another independent consumer appears later, it can read the same aggregated data without changing the update mechanism.