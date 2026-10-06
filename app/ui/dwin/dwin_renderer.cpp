#include "dwin_renderer.hpp"


namespace app
{

namespace
{

// ----------------------------------------------------------------------------
// Action mapping
// ----------------------------------------------------------------------------

constexpr DwinRenderer::ActionMapping action_mappings[] =
{
    {0x2000U, Ui::ActionType::Start},
    {0x2001U, Ui::ActionType::Stop},
    {0x2002U, Ui::ActionType::Edit},
    {0x2004U, Ui::ActionType::Settings},
    {0x2005U, Ui::ActionType::Events},
    {0x2006U, Ui::ActionType::Reset},
    {0x2007U, Ui::ActionType::Previous},
    {0x2008U, Ui::ActionType::Next},
};

// -----------------------------------------------------------------------------
// Screen descriptors
// -----------------------------------------------------------------------------

constexpr DwinRenderer::ScreenDescriptor
    DwinRenderer::screen_descriptors[] =
{
    {
        Ui::Context::Main,
        Ui::Mode::Brief,
        Furnace::State::Idle,
        ScreenId::MainBrief,
        main_brief_fields,
        std::size(main_brief_fields),
        &DwinRenderer::enter_main_brief
    },

    {
        Ui::Context::Main,
        Ui::Mode::Detailed,
        Furnace::State::Count,
        ScreenId::MainDetailed,
        main_detailed_fields,
        std::size(main_detailed_fields),
        &DwinRenderer::enter_main_detailed
    },

    {
        Ui::Context::ProfileSelection,
        Ui::Mode::Start,
        Furnace::State::Count,
        ScreenId::ProfileSelection,
        profile_selection_fields,
        std::size(profile_selection_fields),
        &DwinRenderer::enter_profile_selection
    },

    {
        Ui::Context::ProfileSelection,
        Ui::Mode::Edit,
        Furnace::State::Count,
        ScreenId::ProfileSelection,
        profile_selection_fields,
        std::size(profile_selection_fields),
        &DwinRenderer::enter_profile_selection
    },

    {
        Ui::Context::Settings,
        Ui::Mode::Pid,
        Furnace::State::Count,
        ScreenId::SettingsPid,
        settings_pid_fields,
        std::size(settings_pid_fields),
        &DwinRenderer::enter_settings_pid
    },

    {
        Ui::Context::Settings,
        Ui::Mode::Other,
        Furnace::State::Count,
        ScreenId::SettingsOther,
        settings_other_fields,
        std::size(settings_other_fields),
        &DwinRenderer::enter_settings_other
    },
};

constexpr uint16_t InvalidPage = 0xFFFFU;

} // namespace


// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

void
DwinRenderer::init(
    Ui& ui,
    DataAggregator& data,
    DwinTransport& transport) noexcept
{
    ui_ = &ui;
    data_ = &data;
    transport_ = &transport;

    rendered_context_ = Ui::Context::Count;
    rendered_mode_ = Ui::Mode::Count;

    for (std::size_t i = 0U;
         i < MaxFieldsPerScreen;
         ++i)
    {
        rendered_values_[i] = 0U;
    }
}


// -----------------------------------------------------------------------------
// Main processing
// -----------------------------------------------------------------------------

void
DwinRenderer::update() noexcept
{
    if (ui_ == nullptr ||
        data_ == nullptr ||
        transport_ == nullptr)
    {
        return;
    }

    uint8_t packet[DwinTransport::MaxPacketSize]{};
    std::size_t packet_size = 0U;

    if (transport_->receive(
            packet,
            sizeof(packet),
            packet_size))
    {
        DwinProtocol::TouchEvent event{};

        if (protocol_.decode_touch(
                packet,
                packet_size,
                event))
        {
            Ui::ActionType action = Ui::ActionType::None;

            if (find_action(event.address, action))
            {
                ui_->execute({
                    action,
                    event.value
                });
            }
        }
    }

    const Ui::Position position = ui_->position();

    const ScreenDescriptor* descriptor =
        find_screen_descriptor(
            position.context,
            position.mode);

    if (descriptor == nullptr)
    {
        return;
    }

    const bool position_changed =
        position.context != rendered_context_ ||
        position.mode != rendered_mode_;

    if (position_changed)
    {
        render_screen(*descriptor);

        rendered_context_ = position.context;
        rendered_mode_ = position.mode;

        return;
    }

    update_fields(*descriptor);
}


// -----------------------------------------------------------------------------
// Screen lookup
// -----------------------------------------------------------------------------

const DwinRenderer::ScreenDescriptor*
DwinRenderer::find_screen_descriptor(
    const Ui::Context context,
    const Ui::Mode mode) const noexcept
{
    for (const ScreenDescriptor& descriptor : screen_descriptors)
    {
        if (descriptor.context == context &&
            descriptor.mode == mode)
        {
            return &descriptor;
        }
    }

    return nullptr;
}


// -----------------------------------------------------------------------------
// Action lookup
// -----------------------------------------------------------------------------

bool
DwinRenderer::find_action(
    const uint16_t address,
    Ui::ActionType& action) const noexcept
{
    for (const ActionMapping& mapping : action_mappings)
    {
        if (mapping.address == address)
        {
            action = mapping.action;
            return true;
        }
    }

    action = Ui::ActionType::None;
    return false;
}


// -----------------------------------------------------------------------------
// Data access
// -----------------------------------------------------------------------------

bool
DwinRenderer::get_field_value(
    const FieldMapping& mapping,
    uint16_t& value) const noexcept
{
    switch (mapping.source)
    {
        case DataSource::TcParser:
            value = data_->tc_parser_item(
                static_cast<TcParserItem>(mapping.field));
            return true;

        case DataSource::Furnace:
            value = data_->furnace_item(
                static_cast<FurnaceItem>(mapping.field));
            return true;

        case DataSource::Profile:
            value = data_->profile_item(
                static_cast<ProfileItem>(mapping.field));
            return true;

        case DataSource::Setting:
            value = data_->setting_item(
                static_cast<SettingItem>(mapping.field));
            return true;

        case DataSource::Alarm:
        case DataSource::Count:
            return false;
    }

    return false;
}


// -----------------------------------------------------------------------------
// Screen rendering
// -----------------------------------------------------------------------------

void
DwinRenderer::render_screen(
    const ScreenDescriptor& descriptor) noexcept
{
    switch (descriptor.type)
    {
        case ScreenType::Ordinary:
            render_ordinary(descriptor);
            break;

        case ScreenType::Collection:
            render_collection(descriptor);
            break;

        case ScreenType::Count:
            break;
    }
}


void
DwinRenderer::render_ordinary(
    const ScreenDescriptor& descriptor) noexcept
{
    protocol_.switch_page(descriptor.screen_id);

    for (std::size_t i = 0U;
         i < descriptor.field_count &&
         i < MaxFieldsPerScreen;
         ++i)
    {
        const FieldMapping& field = descriptor.fields[i];

        uint16_t value = 0U;

        if (!get_field_value(field, value))
        {
            continue;
        }

        const DwinProtocol::Packet packet =
            protocol_.write_word(
                field.address,
                value);

        transport_->send(
            packet.data,
            packet.size);

        rendered_values_[i] = value;
    }
}


void
DwinRenderer::render_collection(
    const ScreenDescriptor& descriptor) noexcept
{
    // Collection rendering will be implemented separately.
    // Collection screens map semantic items to physical slots.
    (void)descriptor;
}


// -----------------------------------------------------------------------------
// Incremental field update
// -----------------------------------------------------------------------------

void
DwinRenderer::update_fields(
    const ScreenDescriptor& descriptor) noexcept
{
    if (descriptor.type != ScreenType::Ordinary)
    {
        return;
    }

    for (std::size_t i = 0U;
         i < descriptor.field_count &&
         i < MaxFieldsPerScreen;
         ++i)
    {
        const FieldMapping& field = descriptor.fields[i];

        uint16_t value = 0U;

        if (!get_field_value(field, value))
        {
            continue;
        }

        if (rendered_values_[i] == value)
        {
            continue;
        }

        const DwinProtocol::Packet packet =
            protocol_.write_word(
                field.address,
                value);

        transport_->send(
            packet.data,
            packet.size);

        rendered_values_[i] = value;
    }
}

} // namespace app
