# Problem:
How should data and events be distributed from application sources to consumers?

# Chosen direction:
Notification-based — Sources notify consumers through a callback.

# Rationale:
A single mechanism can handle both data updates and events. The notification identifies the source, notification type, and optional details. The consumer decides whether to use the notification data directly or read additional data from the source.

# Details:
Notification contains:

context — source that generated the notification;
type — what happened;
argument — notification-specific details.

# Example:

TcParser
    |
    | DataReady + temperature
    v
DataAggregator

DataAggregator registers callbacks with application sources and updates its stored data according to the notification type.