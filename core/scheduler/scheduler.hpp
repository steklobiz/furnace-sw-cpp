#pragma once

#include <stdint.h>


namespace core
{

class Scheduler
{
public:

    using TaskHandle = uint8_t;

    using Callback = void (*)(void*);


    static constexpr TaskHandle InvalidTask = 0xff;

    static constexpr uint8_t MaxTasks = 16;

    
    Scheduler();

    //
    // Register a member function as a repeating task.
    //
    // Example:
    //
    //     scheduler.every<Ui, &Ui::process>(
    //         100,
    //         ui_);
    //
    template<typename T, void (T::*Method)()>
    TaskHandle every(
        uint32_t period_ms,
        T& object)
    {
        return add_task(
            period_ms,
            &object,
            trampoline<T, Method>,
            true);
    }



    //
    // Register a normal function as a repeating task.
    //
    // Example:
    //
    //     scheduler.every(
    //         100,
    //         blink);
    //
    TaskHandle every(
        uint32_t period_ms,
        Callback callback);

    // Register a normal function as a one-shot task.
    TaskHandle once(
        uint32_t delay_ms,
        Callback callback);

    // Remove task from scheduler.
    void remove(TaskHandle task);

    // Enable disabled task.
    void enable(TaskHandle task);

    // Disable task without removing.
    void disable(TaskHandle task);

    // Execute ready tasks.
    void run();

private:


    //
    // Converts:
    //
    //     void Class::method()
    //
    // into generic:
    //
    //     void (*)(void*)
    //
    //
    template<typename T, void (T::*Method)()>
    static void trampoline(void* object)
    {
        (static_cast<T*>(object)->*Method)();
    }

    // Common task creation function.
    TaskHandle add_task(
        uint32_t period_ms,
        void* object,
        Callback callback,
        bool repeating);

    struct Task
    {
        // Object owning the callback.
        // nullptr for ordinary functions.
        void* object;

        // Function executed by scheduler.
        Callback callback;

        // Execution interval.
        uint32_t period;
        
        // Next execution time.
        uint32_t next_run;

        // Task state.
        bool enabled;

        // true  = periodic task
        // false = one-shot task
        bool repeating;
    };

    Task tasks_[MaxTasks];
};

} // namespace core