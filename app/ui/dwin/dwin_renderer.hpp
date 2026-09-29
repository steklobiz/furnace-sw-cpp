#pragma once

#include "dwin_protocol.hpp"
#include "dwin_transport.hpp"
#include "ui.hpp"

#include <cstddef>
#include <cstdint>

// Renders application UI state on a DWIN display.
// Converts display-independent Ui data into DWIN-specific presentation.
// Translates DWIN touch events into application UI actions.
// Does not own application state or communicate with the display directly.

namespace app
{

    enum class DwinView : uint8_t
    {
        Main,
        Monitor,
        SettingsPid,
        SettingsOther,
        Count
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
            Start,
            Stop,
            Edit,
            Reset,
            Settings,
            Events,
            Back,
            Previous,
            Next,
            Home
        };

        struct FieldMapping
        {
            uint8_t ui_field;
            uint16_t vp_address;
        };

        using EnterCallback = void (DwinRenderer::*)() noexcept;

        struct ViewDescriptor
        {
            Ui::Page page;
            DwinView view;
            uint16_t dwin_page;
            const FieldMapping* fields;
            std::size_t field_count;
            EnterCallback on_enter;
        };

        DwinAction decode_action(
            uint16_t address,
            uint16_t value) noexcept;

        void handle_action(DwinAction action) noexcept;
        void handle_settings_action(DwinAction action) noexcept;

        void set_view_for_page(Ui::Page page) noexcept;

        void enter_view(const ViewDescriptor& view) noexcept;
        void render_view(
            const ViewDescriptor& view) noexcept;

        void on_enter_settings_pid() noexcept;
        void on_enter_settings_other() noexcept;

        [[nodiscard]] const ViewDescriptor* find_view(
            Ui::Page page,
            DwinView view) const noexcept;

        static constexpr std::size_t MaxRenderedFields = 16;

        static const FieldMapping main_fields[];
        static const FieldMapping monitor_fields[];
        static const FieldMapping settings_pid_fields[];
        static const FieldMapping settings_other_fields[];
        static const ViewDescriptor view_descriptors[];

        Ui* ui_ = nullptr;
        DwinProtocol protocol_{};
        DwinTransport* transport_ = nullptr;

        Ui::Page rendered_page_ = Ui::Page::Count;
        DwinView rendered_view_ = DwinView::Count;
        DwinView view_ = DwinView::Main;

        uint16_t rendered_dwin_page_ = 0xFFFFU;

        uint16_t rendered_values_[MaxRenderedFields]{};
        bool field_rendered_[MaxRenderedFields]{};

    };

} // namespace app