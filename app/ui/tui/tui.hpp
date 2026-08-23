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

    void process_input() noexcept;

    void render_page(
        const PageDescriptor& descriptor,
        Ui::Page page) noexcept;


    Ui* ui_ = nullptr;

    Ui::Page rendered_page_ = Ui::Page::Count;

    bool page_rendered_ = false;

    uint8_t rendered_versions_[
        static_cast<std::size_t>(Ui::Page::Count)][16]{};

    static const DataItem<uint16_t> null_item_;
};

} // namespace app