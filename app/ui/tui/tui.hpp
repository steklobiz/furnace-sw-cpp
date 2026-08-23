// tui.hpp

#pragma once

#include <cstddef>
#include <cstdint>

#include "ui.hpp"

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

    // Process user input and execute matching UI actions.
    void process_input() noexcept;
    
    void process_numeric_input() noexcept;
    
    // Render a regular page using its labels and buttons descriptor.
    void render_page(
        const PageDescriptor& descriptor,
        Ui::Page page) noexcept;
    
    // Render the current step data of the profile being edited.
    void render_profile_content() noexcept;
    
    // Render the complete ProfileEditor page, including its
    // profile data and buttons.
    void render_profile_editor_page() noexcept;
        
    Ui* ui_ = nullptr;

    Ui::Page rendered_page_ = Ui::Page::Count;

    bool page_rendered_ = false;

    uint8_t rendered_versions_[
        static_cast<std::size_t>(Ui::Page::Count)][16]{};

    uint8_t rendered_profile_version_ = 0;
    uint8_t rendered_profile_step_ = 0xff;     
        
    InputMode input_mode_ = InputMode::Normal;
    Ui::ActionType input_action_ = Ui::ActionType::None;

    uint16_t input_value_ = 0;
    bool input_has_value_ = false;
    
    static const DataItem<uint16_t> null_item_;
};

} // namespace app