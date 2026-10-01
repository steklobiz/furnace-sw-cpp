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

    enum class DwinScreen : uint8_t
    {
        Main,
        Monitor,
        ProfileSelection,
        ProfileEditor,
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
            Home,

            Select0,
            Select1,
            Select2,
            Select3,
            Select4,
            Select5,
            Select6,
            Select7,
            Select8,
            Select9
        };

        struct FieldMapping
        {
            uint8_t ui_field;
            uint16_t vp_address;
        };

        using EnterCallback = void (DwinRenderer::*)() noexcept;

        struct ScreenDescriptor
        {
            Ui::Context context;
            DwinScreen screen;
            uint16_t dwin_page_id;
            const FieldMapping* fields;
            std::size_t field_count;
            EnterCallback on_enter;
        };

        DwinAction decode_action(
            uint16_t address,
            uint16_t value) noexcept;

        // Handles DWIN actions according to the current UI context.
        void handle_action(DwinAction action) noexcept;
        void handle_settings_action(DwinAction action) noexcept;
        void handle_main_action(DwinAction action) noexcept;
        void handle_monitor_action(DwinAction action) noexcept;
        void handle_events_action(DwinAction action) noexcept;
        void handle_profile_selection_action(DwinAction action) noexcept;
        void handle_profile_editor_action(DwinAction action) noexcept;

        // Selects the DWIN screen corresponding to the current UI context.
        void set_screen_for_context(Ui::Context context) noexcept;

        void enter_screen(const ScreenDescriptor& descriptor) noexcept;
        void render_screen(
            const ScreenDescriptor& descriptor) noexcept;

        void on_enter_settings_pid() noexcept;
        void on_enter_settings_other() noexcept;

        [[nodiscard]] const ScreenDescriptor* find_screen(
            Ui::Context context,
            DwinScreen screen) const noexcept;

        static constexpr std::size_t MaxRenderedFields = 16;

        static const FieldMapping main_fields[];
        static const FieldMapping monitor_fields[];
        static const FieldMapping settings_pid_fields[];
        static const FieldMapping settings_other_fields[];
        static const FieldMapping profile_selection_fields[];
        static const FieldMapping profile_editor_fields[];

        static const ScreenDescriptor screen_descriptors[];

        Ui* ui_ = nullptr;
        DwinProtocol protocol_{};
        DwinTransport* transport_ = nullptr;

        // Last UI context represented on the DWIN display.
        Ui::Context rendered_context_ = Ui::Context::Count;
        // Last DWIN screen actually rendered.
        DwinScreen rendered_screen_ = DwinScreen::Count;
        // DWIN screen currently selected by the renderer.
        DwinScreen screen_ = DwinScreen::Main;
        // Last DWIN page number actually rendered on the display.
        uint16_t rendered_dwin_page_id_ = 0xFFFFU;

        uint16_t rendered_values_[MaxRenderedFields]{};
        bool field_rendered_[MaxRenderedFields]{};

    };

} // namespace app