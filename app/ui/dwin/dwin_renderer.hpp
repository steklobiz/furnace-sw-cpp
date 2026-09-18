#pragma once

#include "dwin_protocol.hpp"
#include "dwin_transport.hpp"
#include "ui.hpp"

#include <cstddef>
#include <cstdint>

namespace app
{

    enum class DwinView : uint8_t
    {
        Monitor,
        SettingsPid,
        SettingsOther
    };

    // DWIN display renderer.
    // Converts the display-independent Ui state into DWIN-specific presentation.
    // DWIN addresses and display-specific field mappings belong here, not in Ui.
    class DwinRenderer
    {
    public:
        DwinRenderer() noexcept = default;

        // Connects the renderer to the application UI.
        void init(Ui& ui, DwinTransport& transport) noexcept;

        // Processes display input and updates the DWIN presentation.
        void process() noexcept;

    private:
        enum class DwinAction : uint8_t
        {
            None,
            Stop,
            Back,
            Previous,
            Next,
            Home
        };

        DwinAction decode_action(
            uint16_t address,
            uint16_t value) const noexcept;

        void handle_action(DwinAction action) noexcept;

        static constexpr std::size_t MaxRenderedFields = 16;

        void render_page(Ui::Page page) noexcept;

        Ui* ui_ = nullptr;
        DwinProtocol protocol_{};
        DwinTransport* transport_ = nullptr;

        Ui::Page rendered_page_ = Ui::Page::Count;
        DwinView view_ = DwinView::Monitor;

        uint16_t rendered_values_[MaxRenderedFields]{};
        bool field_rendered_[MaxRenderedFields]{};

    };

} // namespace app