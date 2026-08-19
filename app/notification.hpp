// notification.hpp
// Notification defines the common notification contract between
// application data sources and consumers such as DataAggregator.
// A notification identifies its source, describes what happened,
// and may carry a small source-specific argument.

#pragma once

#include <cstdint>

namespace app
{

enum class NotificationType : uint8_t
{
    DataReady,
    Error,
    StepStarted,
    ProfileFinished,
    ProfileChanged,
    // ...
};

struct Notification
{
    void* context;
    NotificationType type;
    uint16_t argument;
};

using NotificationCallback =
    void (*)(void* context, const Notification& notification);
    
} // namespace app