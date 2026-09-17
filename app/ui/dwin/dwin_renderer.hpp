#pragma once

#include <cstddef>
#include <cstdint>

#include "ui.hpp"
#include "dwin_protocol.hpp"

namespace app
{

    // DWIN display renderer.
    // Converts the display-independent Ui state into DWIN-specific presentation.
    // DWIN addresses and display-specific field mappings belong here, not in Ui.
    class DwinRenderer
    {
    public:
        DwinRenderer() noexcept = default;

        // Connects the renderer to the application UI.
        void init(Ui& ui) noexcept;

        // Processes display input and updates the DWIN presentation.
        void process() noexcept;

    private:
        static constexpr std::size_t MaxFieldsPerPage = 10;

        void render_page(Ui::Page page) noexcept;

        Ui* ui_ = nullptr;
        DwinProtocol protocol_{};

        Ui::Page rendered_page_ = Ui::Page::Count;

        
        uint16_t rendered_values_[MaxFieldsPerPage]{};
        bool field_rendered_[MaxFieldsPerPage]{};
    };

} // namespace app