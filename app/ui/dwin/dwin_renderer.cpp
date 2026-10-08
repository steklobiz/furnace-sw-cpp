#include "dwin_renderer.hpp"


namespace app
{

namespace
{

constexpr uint16_t ActionVp = 0x3000U;

constexpr uint16_t ProfileSelectCommandBase = 0x1000U;
constexpr uint16_t ProfileSelectFieldBase   = 0x1300U;


// ----------------------------------------------------------------------------
// Action mapping
// ----------------------------------------------------------------------------

constexpr DwinRenderer::CommandMapping command_mappings[] =
{
    // Main commands
    {0x0000U, Ui::ActionType::Start},
    {0x0002U, Ui::ActionType::Edit},
    {0x0004U, Ui::ActionType::Settings},

    // Navigation
    {0x0010U, Ui::ActionType::Previous},
    {0x0012U, Ui::ActionType::Next},

    // Common
    {0x0020U, Ui::ActionType::Confirm},
    {0x0022U, Ui::ActionType::Cancel},
};

constexpr DwinRenderer::InputMapping input_mappings[] =
{
    // Settings - PID
    {0x0030U, Ui::ActionType::SetPidKp},
    {0x0032U, Ui::ActionType::SetPidKi},
    {0x0034U, Ui::ActionType::SetPidKd},

    // Settings - Other
    {0x0040U, Ui::ActionType::SetMaxTemperature},
    {0x0042U, Ui::ActionType::SetBuzzer},
    {0x0044U, Ui::ActionType::SetPrestepOuts},
};

constexpr DwinRenderer::FieldMapping main_brief_idle_fields[] =
{
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Temperature), 0x1002U},
};

constexpr DwinRenderer::FieldMapping main_brief_running_fields[] =
{
    {DataSource::Profile, static_cast<uint8_t>(ProfileItem::StartProfileId), 0x1000U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Step),          0x1001U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Temperature),   0x1002U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Power),         0x1003U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Outputs),       0x1004U},
};

constexpr DwinRenderer::FieldMapping main_brief_auto_fields[] =
{
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Temperature), 0x1002U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Step),          0x1001U},
};

constexpr DwinRenderer::FieldMapping main_brief_waiting_fields[] =
{
    {DataSource::Profile, static_cast<uint8_t>(ProfileItem::StartProfileId), 0x1000U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Outputs),       0x1004U},
};

constexpr DwinRenderer::FieldMapping main_brief_stopped_fields[] =
{
    {DataSource::Profile, static_cast<uint8_t>(ProfileItem::StartProfileId), 0x1000U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Outputs),       0x1004U},
};

constexpr DwinRenderer::FieldMapping main_brief_finished_fields[] =
{
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Temperature), 0x1002U},
};

constexpr DwinRenderer::FieldMapping main_brief_error_fields[] =
{
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Temperature), 0x1002U},
};


constexpr DwinRenderer::FieldMapping main_detailed_fields[] =
{
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Step),          0x1102U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::StepType),      0x1103U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Temperature),   0x1104U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Setpoint),      0x1105U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::StepElapsed),   0x1106U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::ProfileElapsed),0x1107U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Power),         0x1108U},
    {DataSource::Furnace, static_cast<uint8_t>(FurnaceItem::Outputs),       0x1109U},
};

constexpr DwinRenderer::FieldMapping settings_pid_fields[] =
{
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::PidKp), 0x1200U},
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::PidKi), 0x1201U},
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::PidKd), 0x1202U},
};

constexpr DwinRenderer::FieldMapping settings_other_fields[] =
{
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::Buzzer),       0x1210U},
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::MaxTemperature), 0x1211U},
    {DataSource::Setting, static_cast<uint8_t>(SettingItem::PrestepOuts),  0x1212U},
};

// -----------------------------------------------------------------------------
// Screen descriptors
// -----------------------------------------------------------------------------

constexpr DwinRenderer::ScreenDescriptor screen_descriptors[] =
{
    // Main / Brief / Idle
    {
        Ui::Context::Main,
        Ui::Mode::Brief,
        Furnace::State::Idle,
        DwinRenderer::ScreenId::MainBriefIdle,
        main_brief_idle_fields,
        std::size(main_brief_idle_fields),
        nullptr
    },

    // Main / Brief / Running
    {
        Ui::Context::Main,
        Ui::Mode::Brief,
        Furnace::State::Running,
        DwinRenderer::ScreenId::MainBriefRunning,
        main_brief_running_fields,
        std::size(main_brief_running_fields),
        nullptr
    },

    // Main / Brief / Auto
    {
        Ui::Context::Main,
        Ui::Mode::Brief,
        Furnace::State::Auto,
        DwinRenderer::ScreenId::MainBriefAuto,
        main_brief_auto_fields,
        std::size(main_brief_auto_fields),
        nullptr
    },

    // Main / Brief / Waiting
    {
        Ui::Context::Main,
        Ui::Mode::Brief,
        Furnace::State::Waiting,
        DwinRenderer::ScreenId::MainBriefWaiting,
        main_brief_waiting_fields,
        std::size(main_brief_waiting_fields),
        nullptr
    },

    // Main / Brief / Stopped
    {
        Ui::Context::Main,
        Ui::Mode::Brief,
        Furnace::State::Stopped,
        DwinRenderer::ScreenId::MainBriefStopped,
        main_brief_stopped_fields,
        std::size(main_brief_stopped_fields),
        nullptr
    },

    // Main / Brief / Finished
    {
        Ui::Context::Main,
        Ui::Mode::Brief,
        Furnace::State::Finished,
        DwinRenderer::ScreenId::MainBriefFinished,
        main_brief_finished_fields,
        std::size(main_brief_finished_fields),
        nullptr
    },

    // Main / Brief / Error
    {
        Ui::Context::Main,
        Ui::Mode::Brief,
        Furnace::State::Error,
        DwinRenderer::ScreenId::MainBriefError,
        main_brief_error_fields,
        std::size(main_brief_error_fields),
        nullptr
    },

    // Main / Detailed
    {
        Ui::Context::Main,
        Ui::Mode::Detailed,
        Furnace::State::Count,
        DwinRenderer::ScreenId::MainDetailed,
        main_detailed_fields,
        std::size(main_detailed_fields),
        nullptr
    },

    // Profile selection / Start
    {
        Ui::Context::ProfileSelection,
        Ui::Mode::Start,
        Furnace::State::Count,
        DwinRenderer::ScreenId::ProfileSelection,
        nullptr,
        0U,
        &DwinRenderer::enter_profile_selection
    },

    // Profile selection / Edit
    {
        Ui::Context::ProfileSelection,
        Ui::Mode::Edit,
        Furnace::State::Count,
        DwinRenderer::ScreenId::ProfileSelection,
        nullptr,
        0U,
        &DwinRenderer::enter_profile_selection
    },

    // Settings / PID
    {
        Ui::Context::Settings,
        Ui::Mode::Pid,
        Furnace::State::Count,
        DwinRenderer::ScreenId::SettingsPid,
        settings_pid_fields,
        std::size(settings_pid_fields),
        nullptr
    },

    // Settings / Other
    {
        Ui::Context::Settings,
        Ui::Mode::Other,
        Furnace::State::Count,
        DwinRenderer::ScreenId::SettingsOther,
        settings_other_fields,
        std::size(settings_other_fields),
        nullptr
    },

    // Question / Stop
    {
        Ui::Context::Question,
        Ui::Mode::Stop,
        Furnace::State::Count,
        DwinRenderer::ScreenId::Question,
        nullptr,
        0U,
        nullptr
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
    rendered_state_ = Furnace::State::Count;
    rendered_profile_page_ = 0xFFU;

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
            uint16_t argument = 0U;

            // ActionVp is a common command VP. Its value identifies
            // the command and, for profile selection, the slot.
            if (event.address == ActionVp)
            {
                if (find_command(event.value, action, argument))
                {
                    ui_->execute({
                        action,
                        argument
                    });
                }
            }
            // Other VPs identify an action directly. Their value
            // is passed to the UI as the action argument.
            else if (find_input_action(event.address, action))
            {
                ui_->execute({
                    action,
                    event.value
                });
            }
        }
    }

    const Ui::Position position = ui_->position();
    const Furnace::State state = ui_->state();

    const ScreenDescriptor* descriptor =
        find_screen_descriptor(
            position.context,
            position.mode,
            state);

    if (descriptor == nullptr)
    {
        return;
    }

    // A screen is rendered completely when the semantic UI position
    // or Furnace state changes. Otherwise only changed field values
    // are sent to the DWIN display.
    const bool position_changed =
        position.context != rendered_context_ ||
        position.mode != rendered_mode_ ||
        state != rendered_state_;

    const bool profile_page_changed =
        position.context == Ui::Context::ProfileSelection &&
        ui_->profile_page() != rendered_profile_page_;

    if (position_changed)
    {
        render_screen(*descriptor);

        rendered_context_ = position.context;
        rendered_mode_ = position.mode;
        rendered_state_ = state;

        return;
    }

    if (profile_page_changed)
    {
        enter_profile_selection(*this);
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
    const Ui::Mode mode,
    const Furnace::State state) const noexcept
{
    for (const ScreenDescriptor& descriptor : screen_descriptors)
    {
        if (descriptor.context != context ||
            descriptor.mode != mode)
        {
            continue;
        }

        if (descriptor.state == state ||
            descriptor.state == Furnace::State::Count)
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
DwinRenderer::find_command(
    const uint16_t value,
    Ui::ActionType& action,
    uint16_t& argument) const noexcept
{
    if (value >= ProfileSelectCommandBase &&
        value < ProfileSelectCommandBase + ProfilesPerPage)
    {
        action = Ui::ActionType::Select;
        argument = static_cast<uint16_t>(
            value - ProfileSelectCommandBase);

        return true;
    }

    for (const CommandMapping& mapping : command_mappings)
    {
        if (mapping.value == value)
        {
            action = mapping.action;
            argument = 0U;

            return true;
        }
    }

    action = Ui::ActionType::None;
    argument = 0U;

    return false;
}

bool
DwinRenderer::find_input_action(
    const uint16_t address,
    Ui::ActionType& action) const noexcept
{
    for (const InputMapping& mapping : input_mappings)
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
// Screen rendering and updating
// -----------------------------------------------------------------------------

void
DwinRenderer::render_screen(
    const ScreenDescriptor& descriptor) noexcept
{
    for (std::size_t i = 0U; i < MaxFieldsPerScreen; ++i)
    {
        rendered_values_[i] = 0xFFFFU;
    }

    const DwinProtocol::Packet packet =
        protocol_.switch_page(
            static_cast<uint16_t>(descriptor.screen_id));

    transport_->send(
        packet.data,
        packet.size);

    update_fields(descriptor);

    if (descriptor.enter != nullptr)
    {
        descriptor.enter(*this);
    }
}

void
DwinRenderer::update_fields(
    const ScreenDescriptor& descriptor) noexcept
{
    for (std::size_t i = 0U;
         i < descriptor.field_count &&
         i < MaxFieldsPerScreen;
         ++i)
    {
        const FieldMapping& field = descriptor.fields[i];

        const uint16_t value =
            data_->item(field.source, field.field);

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

// -----------------------------------------------------------------------------
// Enter functions
// -----------------------------------------------------------------------------

void
DwinRenderer::enter_profile_selection(
    DwinRenderer& renderer) noexcept
{
    const uint8_t page = renderer.ui_->profile_page();
    const std::size_t profile_count =
        renderer.ui_->profile_count();

    const std::size_t first_profile =
        static_cast<std::size_t>(page) *
        ProfilesPerPage;

    for (uint8_t slot = 0U;
         slot < ProfilesPerPage;
         ++slot)
    {
        const std::size_t profile_id =
            first_profile + slot;

        const uint16_t address =
            static_cast<uint16_t>(
                ProfileSelectFieldBase + slot);

        uint16_t value = 0U;

        if (profile_id < profile_count)
        {
            value = static_cast<uint16_t>(profile_id);
        }

        const DwinProtocol::Packet packet =
            renderer.protocol_.write_word(
                address,
                value);

        renderer.transport_->send(
            packet.data,
            packet.size);
    }

    renderer.rendered_profile_page_ = page;
}

} // namespace app
