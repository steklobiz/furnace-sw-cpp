#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

namespace core
{

template<
    class Owner,
    class State,
    class Event>
class Fsm
{
public:

    static_assert(std::is_enum_v<State>);
//    static_assert(std::is_enum_v<Event>);

    using StateHandler =
        State (Owner::*)(const Event&) noexcept;

    using EnterHandler =
        void (Owner::*)() noexcept;

    using ExitHandler =
        void (Owner::*)() noexcept;

    static constexpr std::size_t StateCount =
        static_cast<std::size_t>(State::Count);

    using StateTable =
        std::array<StateHandler, StateCount>;

    using EnterTable =
        std::array<EnterHandler, StateCount>;

    using ExitTable =
        std::array<ExitHandler, StateCount>;

    struct Tables
    {
        StateTable handlers;

        const EnterTable* enters = nullptr;
        const ExitTable* exits = nullptr;
    };
        
    // init() replaces the constructor
    constexpr void init(
        Owner& owner,
        State initial,
        const Tables& tables) noexcept
    {
        owner_ = &owner;
        state_ = initial;
        handlers_ = &tables.handlers;
        enters_ = tables.enters;
        exits_ = tables.exits;
        
        // Execute enter callback of initial state
        if (enters_)
        {
            if (const auto enter = (*enters_)[index(state_)])
                (owner_->*enter)();
        }
    }
        
    State state() const noexcept
    {
        return state_;
    }

    void dispatch(const Event& event) noexcept
    {
                 
        const auto handler =
            handlers_->operator[](index(state_));

        transition((owner_->*handler)(event));
    }

    void set_state(State next) noexcept
    {
        transition(next);
    }

private:

    static constexpr std::size_t 
    index(State state) noexcept
    {
        return static_cast<std::size_t>(state);
    }

    void 
    transition(State next) noexcept
    {
        if (next == state_)
            return;

        // Call Exit for an old ctate    
        if (exits_)
        {
            if (const auto exit = (*exits_)[index(state_)])
                (owner_->*exit)();
        }

        state_ = next;

        // Call Enter for new state
        if (enters_)
        {
            if (const auto enter = (*enters_)[index(state_)])
                (owner_->*enter)();
        }
    }
    
    Owner* owner_ = nullptr;

    State state_;

    const StateTable* handlers_ = nullptr;

    const EnterTable* enters_ = nullptr;

    const ExitTable* exits_ = nullptr; 
};

} // namespace core