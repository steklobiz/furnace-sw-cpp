Problem:
How should Error Directives be distributed from multiple producers to multiple consumers?

Chosen direction:
Error Dispatcher — Producers call trigger(); Dispatcher forwards to all registered consumers

Rationale:
M:N topology requires centralized routing to avoid duplication in each producer
