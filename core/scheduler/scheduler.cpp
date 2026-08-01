#include "scheduler.hpp"
#include "hal.hpp"

namespace core
{

Scheduler::Scheduler()
{
    for (auto& task : tasks_)
    {
        task.object = nullptr;
        task.callback = nullptr;

        task.period = 0;
        task.next_run = 0;

        task.enabled = false;
        task.repeating = false;
    }
}


Scheduler::TaskHandle Scheduler::every(
    uint32_t period,
    Callback callback)
{
    return add_task(
        period,
        nullptr,
        callback,
        true);
}


Scheduler::TaskHandle Scheduler::once(
    uint32_t delay,
    Callback callback)
{
    return add_task(
        delay,
        nullptr,
        callback,
        false);
}


Scheduler::TaskHandle Scheduler::add_task(
    uint32_t period,
    void* object,
    Callback callback,
    bool repeating)
{
    for (TaskHandle i = 0; i < MaxTasks; ++i)
    {
        if (tasks_[i].callback != nullptr)
            continue;


        tasks_[i].object = object;

        tasks_[i].callback = callback;

        tasks_[i].period = period;

        tasks_[i].next_run = hal::tick_ms() + period;

        tasks_[i].enabled = true;

        tasks_[i].repeating = repeating;


        return i;
    }


    return InvalidTask;
}


void Scheduler::remove(TaskHandle task)
{
    if (task >= MaxTasks)
        return;


    tasks_[task].object = nullptr;

    tasks_[task].callback = nullptr;

    tasks_[task].enabled = false;
}


void Scheduler::enable(TaskHandle task)
{
    if (task >= MaxTasks)
        return;


    if (tasks_[task].callback == nullptr)
        return;


    tasks_[task].enabled = true;
}


void Scheduler::disable(TaskHandle task)
{
    if (task >= MaxTasks)
        return;


    tasks_[task].enabled = false;
}


void Scheduler::run()
{
    const uint32_t now = hal::tick_ms();


    for (Task& task : tasks_)
    {
        if (task.callback == nullptr)
            continue;


        if (!task.enabled)
            continue;


        if (static_cast<int32_t>(now - task.next_run) < 0)
            continue;



        if (task.repeating)
        {
            task.next_run += task.period;
        }
        else
        {
            task.enabled = false;
        }


        task.callback(task.object);
    }
}


} // namespace core