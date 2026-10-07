#pragma once

#include <cstddef>
#include <cstdint>

#include "data_aggregator.hpp"
#include "ui.hpp"
#include "dwin_protocol.hpp"
#include "dwin_transport.hpp"

// DWIN renderer: translates UI positions and application data into DWIN screen output.
// Maps semantic UI actions to DWIN touch inputs and updates only changed field values.
// Owns renderer-specific screen descriptors and presentation cache.
// Does not own UI semantics, application data, or DWIN transport.

namespace app
{

class DwinRenderer
{
public:
    enum class ScreenId : uint8_t
    {
        MainBriefIdle,      // 0
        MainBriefRunning,   // 1
        MainBriefAuto,      // 2
        MainBriefWaiting,   // 3
        MainBriefStopped,   // 4
        MainBriefFinished,  // 5
        MainBriefError,     // 6
        MainDetailed,       // 7
        ProfileSelection,   // 8
        SettingsPid,        // 9
        SettingsOther,      // A
        Question,
        Count
    };

    struct FieldMapping
    {
        DataSource source;
        uint8_t field;
        uint16_t address;
    };

    struct CommandMapping
    {
        uint16_t value;
        Ui::ActionType action;
    };

    struct InputMapping
    {
        uint16_t address;
        Ui::ActionType action;
    };

    struct ScreenDescriptor
    {
        Ui::Context context;
        Ui::Mode mode;
        Furnace::State state;

        ScreenId screen_id;

        const FieldMapping* fields;
        std::size_t field_count;

        void (*enter)(DwinRenderer&) noexcept;
    };

    DwinRenderer() noexcept = default;

    void init(
        Ui& ui,
        DataAggregator& data,
        DwinTransport& transport) noexcept;

    void update() noexcept;

    static void enter_profile_selection(DwinRenderer& renderer) noexcept;


private:
    static constexpr std::size_t MaxFieldsPerScreen = 16U;
    static constexpr uint8_t ProfilesPerPage = 10U;

    const ScreenDescriptor* find_screen_descriptor(
        Ui::Context context,
        Ui::Mode mode,
        Furnace::State state) const noexcept;

    void render_screen(
        const ScreenDescriptor& descriptor) noexcept;

    void update_fields(
        const ScreenDescriptor& descriptor) noexcept;

    bool find_command(
    uint16_t value, Ui::ActionType& action, uint16_t& argument) const noexcept;

    bool find_input_action(
        const uint16_t address, Ui::ActionType& action) const noexcept;

    Ui* ui_{nullptr};
    DataAggregator* data_{nullptr};
    DwinTransport* transport_{nullptr};

    DwinProtocol protocol_;

    // Semantic position/state of the screen rendered during the previous update.
    // Count means that no screen has been rendered yet.
    Ui::Context rendered_context_{Ui::Context::Count};
    Ui::Mode rendered_mode_{Ui::Mode::Count};
    Furnace::State rendered_state_{Furnace::State::Count};

    // Profile-selection page rendered during the previous update.
    // 0xFF means that no profile-selection page has been rendered yet.
    uint8_t rendered_profile_page_{0xFFU};

    // Last values written to the DWIN VPs for the current screen.
    // Used to avoid sending unchanged field values.
    uint16_t rendered_values_[MaxFieldsPerScreen]{};

    // static constexpr ActionMapping action_mappings[]{};
    // static constexpr ScreenDescriptor screen_descriptors[]{};
};

} // namespace app
