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
        MainBrief,      // 0
        MainDetailed,   // 1
        Result,         // 2
        ProfileSelection,
        SettingsPid,
        SettingsOther,
        Count
    };

    static constexpr std::size_t MaxFieldsPerScreen = 16;

    enum class ScreenType : uint8_t
    {
        Ordinary,
        Collection,
        Count
    };

    struct FieldMapping
    {
        DataSource source;
        uint8_t field;
        uint16_t address;
    };

    struct ActionMapping
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

private:
    const ScreenDescriptor* find_screen_descriptor(
        Ui::Context context,
        Ui::Mode mode) const noexcept;

    bool get_field_value(
        const FieldMapping& mapping,
        uint16_t& value) const noexcept;

    void render_screen(
        const ScreenDescriptor& descriptor) noexcept;

    void render_ordinary(
        const ScreenDescriptor& descriptor) noexcept;

    void render_collection(
        const ScreenDescriptor& descriptor) noexcept;

    void update_fields(
        const ScreenDescriptor& descriptor) noexcept;

    bool find_action(
        uint16_t address,
        Ui::ActionType& action) const noexcept;

    Ui* ui_{nullptr};
    DataAggregator* data_{nullptr};
    DwinTransport* transport_{nullptr};

    DwinProtocol protocol_;

    Ui::Context rendered_context_{Ui::Context::Count};
    Ui::Mode rendered_mode_{Ui::Mode::Count};

    uint16_t rendered_values_[MaxFieldsPerScreen]{};

    // static constexpr ActionMapping action_mappings[]{};
    // static constexpr ScreenDescriptor screen_descriptors[]{};
};

} // namespace app
