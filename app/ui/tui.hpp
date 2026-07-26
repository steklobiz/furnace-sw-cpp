#pragma once

#include "ui.hpp"

namespace app
{

class Tui
{
public:

    explicit Tui(Ui& ui);

    void process() noexcept;

private:

    void render() noexcept;

    void show_main() noexcept;
    void show_monitor() noexcept;
    void show_profile_select() noexcept;

    void process_input() noexcept; 
    void process_main_input() noexcept;
    void process_monitor_input() noexcept;
    void process_profile_select_input() noexcept;
    
    static const char* action_name(
        Ui::Button::Action action) noexcept;

    static const char* state_name(
        Furnace::State state) noexcept;

    static const char* step_type_name(
        Furnace::StepType type) noexcept;
               
    Ui::Event::Id event_from_action(
        Ui::Button::Action action) noexcept;
                    
private:

    Ui& ui_;
    
};

} // namespace app