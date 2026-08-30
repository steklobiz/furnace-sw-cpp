// tui.hpp

#pragma once

#include <cstddef>
#include <cstdint>

#include "ui.hpp"
#include "profiles.hpp"

// TUI renderer for the application UI.
// Handles terminal input, page navigation, and numeric input.
// Renders UI data and updates only fields whose displayed values changed.
// Keeps renderer-specific descriptors and rendering state internal to the TUI.

namespace app
{

class Tui
{
public:

    enum class InputMode : uint8_t
    {
        Normal,
        Numeric
    };


    struct Label
    {
        const char* caption;
        uint8_t field;
    };


    struct Button
    {
        char key;
        const char* caption;
        Ui::ActionType action;
        uint16_t argument;
        bool numeric_input = false;
    };


    struct PageDescriptor
    {
        const Label* labels;
        std::size_t label_count;

        const Button* buttons;
        std::size_t button_count;
    };


    void init(Ui& ui) noexcept;

    void process() noexcept;


private:

    // Maximum number of fields rendered by any regular page.
    static constexpr std::size_t MaxFieldsPerPage = 10;

    static constexpr std::size_t MaxEventsPerPage = 10;
    
    // Process user input and execute matching UI actions.
    void process_input() noexcept;
    
    // Process numeric input for buttons marked as numeric.
    void process_numeric_input() noexcept;
    
    // Render a regular page using its labels and buttons descriptor.
    void render_page(
        const PageDescriptor& descriptor,
        Ui::Page page) noexcept;
    
    // Common button rendering.    
    void render_buttons(
        const PageDescriptor& descriptor,
        std::size_t first_row) noexcept;    
        
    // Render the current step data of the profile being edited.
    void render_profile_content() noexcept;
    
    // Render the complete ProfileEditor page, including its
    // profile data and buttons.
    void render_profile_editor_page() noexcept;
    
    void render_settings_page() noexcept;
    
    void render_events_page() noexcept;
    
    
    // UI model used as the source for rendering and input actions.    
    Ui* ui_ = nullptr;

    // Page that was rendered during the previous render cycle.
    Ui::Page rendered_page_ = Ui::Page::Count;
    
    // Indicates whether the current page has been rendered at least once.
    bool page_rendered_ = false;
     
    // Last values rendered for each field of each page.   
    uint16_t rendered_values_[
        static_cast<std::size_t>(Ui::Page::Count)][MaxFieldsPerPage]{};
    
    // Last profile step rendered by the profile editor.    
    Step rendered_step_{};
    uint8_t rendered_step_index_ = 0xff;
    
    // Current input mode.
    InputMode input_mode_ = InputMode::Normal;
    // Action associated with the current numeric input.
    Ui::ActionType input_action_ = Ui::ActionType::None;
    
    // Accumulated numeric input value.
    uint16_t input_value_ = 0;
    // Indicates whether at least one numeric digit has been entered.
    bool input_has_value_ = false;
};

} // namespace app