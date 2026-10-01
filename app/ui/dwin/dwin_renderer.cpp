#include "dwin_renderer.hpp"

namespace app
{

const DwinRenderer::FieldMapping DwinRenderer::main_fields[] = {
    {0U, 0x1000U}, // State
    {1U, 0x1001U}, // Profile
    {2U, 0x1002U}, // Temperature
    // Step
    // Power
    // Outputs
};

const DwinRenderer::FieldMapping DwinRenderer::monitor_fields[] = {
    {0U, 0x1100U}, // State
    {1U, 0x1101U}, // Profile
    {2U, 0x1102U}, // Step
    {3U, 0x1103U}, // Step type
    {4U, 0x1104U}, // Temperature
    {5U, 0x1105U}, // Setpoint
    {6U, 0x1106U}, // Step elapsed
    {7U, 0x1107U}, // Profile elapsed
    {8U, 0x1108U}, // Power
    {9U, 0x1109U}, // Outputs
};

const DwinRenderer::FieldMapping DwinRenderer::settings_pid_fields[] = {
    {1U, 0x1200U}, // PID Kp
    {2U, 0x1201U}, // PID Ki
    {3U, 0x1202U}, // PID Kd
};

const DwinRenderer::FieldMapping DwinRenderer::settings_other_fields[] = {
    {0U, 0x1210U}, // Buzzer
    {4U, 0x1211U}, // Max temperature
    {5U, 0x1212U}, // Prestep outputs
};

const DwinRenderer::ScreenDescriptor DwinRenderer::screen_descriptors[] = {
    {
        Ui::Context::Main,
        DwinScreen::Main,
        0U,
        main_fields,
        std::size(main_fields),
        nullptr
    },
    {
        Ui::Context::Monitor,
        DwinScreen::Monitor,
        1U,
        monitor_fields,
        std::size(monitor_fields),
        nullptr
    },
    {
        Ui::Context::Settings,
        DwinScreen::SettingsPid,
        2U,
        settings_pid_fields,
        std::size(settings_pid_fields),
        &DwinRenderer::on_enter_settings_pid
    },
    {
        Ui::Context::Settings,
        DwinScreen::SettingsOther,
        3U,
        settings_other_fields,
        std::size(settings_other_fields),
        &DwinRenderer::on_enter_settings_other
    },
    {
        Ui::Context::ProfileSelection,
        DwinScreen::ProfileSelection,
        4U,
        nullptr,
        0U,
        nullptr
    },
};

DwinRenderer::DwinAction DwinRenderer::decode_action(
    uint16_t address,
    uint16_t value) noexcept
{
    (void)value;

    switch (address)
    {
        case 0x2000U: return DwinAction::Start;
        case 0x2001U: return DwinAction::Stop;
        case 0x2002U: return DwinAction::Edit;
        case 0x2003U: return DwinAction::Back;
        case 0x2004U: return DwinAction::Settings;
        case 0x2005U: return DwinAction::Events;
        case 0x2006U: return DwinAction::Reset;
        case 0x2007U: return DwinAction::Previous;
        case 0x2008U: return DwinAction::Next;
        case 0x2009U: return DwinAction::Home;
        default: return DwinAction::None;
    }
}

const DwinRenderer::ScreenDescriptor* DwinRenderer::find_screen(
    Ui::Context context,
    DwinScreen screen) const noexcept
{
    for (const ScreenDescriptor& descriptor : screen_descriptors)
    {
        if (descriptor.context == context && descriptor.screen == screen)
        {
            return &descriptor;
        }
    }

    return nullptr;
}


void DwinRenderer::init(
    Ui& ui,
    DwinTransport& transport) noexcept
{
    ui_ = &ui;
    transport_ = &transport;
    rendered_context_ = Ui::Context::Count;
    rendered_screen_ = DwinScreen::Count;
    rendered_dwin_page_id_ = 0xFFFFU;
    screen_ = DwinScreen::Main;

    for (std::size_t i = 0; i < MaxRenderedFields; ++i)
    {
        rendered_values_[i] = 0U;
        field_rendered_[i] = false;
    }
}

void DwinRenderer::process() noexcept
{
    if (ui_ == nullptr || transport_ == nullptr)
    {
        return;
    }

    uint8_t data[DwinProtocol::MaxPacketSize]{};
    std::size_t size = 0U;

    if (transport_->receive(data, sizeof(data), size))
    {
        DwinProtocol::TouchEvent event{};

        if (protocol_.decode_touch(data, size, event))
        {
            const DwinAction action = decode_action(event.address, event.value);

            if (action != DwinAction::None)
            {
                handle_action(action);
            }
        }
    }

    const Ui::Context context = ui_->context();

    if (context != rendered_context_)
    {
        set_screen_for_context(context);
    }

    const ScreenDescriptor* screen = find_screen(context, screen_);

    if (screen == nullptr)
    {
        return;
    }

    if (context != rendered_context_ || screen_ != rendered_screen_)
    {
        enter_screen(*screen);
        rendered_context_ = context;
        rendered_screen_ = screen_;
        return;
    }

    render_screen(*screen);
}



void DwinRenderer::set_screen_for_context(Ui::Context context) noexcept
{
    switch (context)
    {
        case Ui::Context::Main:
            screen_ = DwinScreen::Main;
            break;

        case Ui::Context::Monitor:
            screen_ = DwinScreen::Monitor;
            break;

        case Ui::Context::Settings:
            if (screen_ != DwinScreen::SettingsPid &&
                screen_ != DwinScreen::SettingsOther)
            {
                screen_ = DwinScreen::SettingsPid;
            }
            break;

        default:
            break;
    }
}

void DwinRenderer::enter_screen(const ScreenDescriptor& descriptor) noexcept
{
    if (descriptor.dwin_page != rendered_dwin_page_id_)
    {
        const DwinProtocol::Packet packet =
            protocol_.switch_page(descriptor.dwin_page);
        transport_->send(packet.data, packet.size);
        rendered_dwin_page_id_ = descriptor.dwin_page;
    }

    if (descriptor.on_enter != nullptr)
    {
        (this->*descriptor.on_enter)();
    }

    for (std::size_t i = 0; i < MaxRenderedFields; ++i)
    {
        field_rendered_[i] = false;
    }

    render_screen(descriptor);
}

void DwinRenderer::render_screen(const ScreenDescriptor& descriptor) noexcept
{
    for (std::size_t i = 0; i < descriptor.field_count; ++i)
    {
        const FieldMapping& field = descriptor.fields[i];

        if (field.ui_field >= MaxRenderedFields)
        {
            continue;
        }

        uint16_t value = 0U;

        if (!ui_->get_field(descriptor.context, field.ui_field, value))
        {
            continue;
        }

        if (field_rendered_[field.ui_field] &&
            rendered_values_[field.ui_field] == value)
        {
            continue;
        }

        rendered_values_[field.ui_field] = value;
        field_rendered_[field.ui_field] = true;

        const DwinProtocol::Packet packet =
            protocol_.write_word(field.vp_address, value);

        transport_->send(packet.data, packet.size);
    }
}

void DwinRenderer::on_enter_settings_pid() noexcept
{
    // Reserved for Settings/PID-specific initialization.
}

void DwinRenderer::on_enter_settings_other() noexcept
{
    // Reserved for Settings/Other-specific initialization.
}


void DwinRenderer::handle_action(DwinAction action) noexcept
{
    switch (ui_->context())
    {
        case Ui::Context::Main:
            handle_main_action(action);
            break;

        case Ui::Context::Monitor:
            handle_monitor_action(action);
            break;

        case Ui::Context::Settings:
            handle_settings_action(action);
            break;

        case Ui::Context::Events:
            handle_events_action(action);
            break;

        default:
            break;
    }
}

void DwinRenderer::handle_main_action(DwinAction action) noexcept
{
    switch (action)
    {
        case DwinAction::Start:
            ui_->execute({Ui::ActionType::StartProfileSelection, 0U});
            break;
        case DwinAction::Edit:
            ui_->execute({Ui::ActionType::EditProfileSelection, 0U});
            break;
        case DwinAction::Settings:
            ui_->execute({Ui::ActionType::Settings, 0U});
            break;
        case DwinAction::Events:
            ui_->execute({Ui::ActionType::ShowEvents, 0U});
            break;
        default:
            break;
    }
}

void DwinRenderer::handle_monitor_action(DwinAction action) noexcept
{
    switch (action)
    {
        case DwinAction::Stop:
            ui_->execute({Ui::ActionType::StopFurnace, 0U});
            break;
        case DwinAction::Back:
            ui_->execute({Ui::ActionType::Back, 0U});
            break;
        default:
            break;
    }
}

void DwinRenderer::handle_events_action(DwinAction action) noexcept
{
}

void DwinRenderer::handle_settings_action(DwinAction action) noexcept
{
    switch (action)
    {
        case DwinAction::Previous:
        case DwinAction::Next:
            screen_ = (screen_ == DwinScreen::SettingsPid)
                ? DwinScreen::SettingsOther
                : DwinScreen::SettingsPid;
            break;

        case DwinAction::Back:
            ui_->execute({Ui::ActionType::Back, 0U});
            break;

        default:
            break;
    }
}

} // namespace app