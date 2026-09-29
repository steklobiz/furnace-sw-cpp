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

const DwinRenderer::ViewDescriptor DwinRenderer::view_descriptors[] = {
    {
        Ui::Page::Main,
        DwinView::Main,
        0U,
        main_fields,
        std::size(main_fields),
        nullptr
    },
    {
        Ui::Page::Monitor,
        DwinView::Monitor,
        1U,
        monitor_fields,
        std::size(monitor_fields),
        nullptr
    },
    {
        Ui::Page::Settings,
        DwinView::SettingsPid,
        2U,
        settings_pid_fields,
        std::size(settings_pid_fields),
        &DwinRenderer::on_enter_settings_pid
    },
    {
        Ui::Page::Settings,
        DwinView::SettingsOther,
        3U,
        settings_other_fields,
        std::size(settings_other_fields),
        &DwinRenderer::on_enter_settings_other
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

const DwinRenderer::ViewDescriptor* DwinRenderer::find_view(
    Ui::Page page,
    DwinView view) const noexcept
{
    for (const ViewDescriptor& descriptor : view_descriptors)
    {
        if (descriptor.page == page && descriptor.view == view)
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
    rendered_page_ = Ui::Page::Count;
    rendered_view_ = DwinView::Main;
    rendered_dwin_page_ = 0xFFFFU;
    view_ = DwinView::Main;

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

    const Ui::Page page = ui_->page();

    if (page != rendered_page_)
    {
        set_view_for_page(page);
    }

    const ViewDescriptor* view = find_view(page, view_);

    if (view == nullptr)
    {
        return;
    }

    if (page != rendered_page_ || view_ != rendered_view_)
    {
        enter_view(*view);
        rendered_page_ = page;
        rendered_view_ = view_;
        return;
    }

    render_view(*view);
}



void DwinRenderer::set_view_for_page(Ui::Page page) noexcept
{
    switch (page)
    {
        case Ui::Page::Main:
            view_ = DwinView::Main;
            break;

        case Ui::Page::Monitor:
            view_ = DwinView::Monitor;
            break;

        case Ui::Page::Settings:
            if (view_ != DwinView::SettingsPid &&
                view_ != DwinView::SettingsOther)
            {
                view_ = DwinView::SettingsPid;
            }
            break;

        default:
            break;
    }
}

void DwinRenderer::enter_view(const ViewDescriptor& view) noexcept
{
    if (view.dwin_page != rendered_dwin_page_)
    {
        const DwinProtocol::Packet packet =
            protocol_.switch_page(view.dwin_page);
        transport_->send(packet.data, packet.size);
        rendered_dwin_page_ = view.dwin_page;
    }

    if (view.on_enter != nullptr)
    {
        (this->*view.on_enter)();
    }

    for (std::size_t i = 0; i < MaxRenderedFields; ++i)
    {
        field_rendered_[i] = false;
    }

    render_view(view);
}

void DwinRenderer::render_view(const ViewDescriptor& view) noexcept
{
    for (std::size_t i = 0; i < view.field_count; ++i)
    {
        const FieldMapping& field = view.fields[i];

        if (field.ui_field >= MaxRenderedFields)
        {
            continue;
        }

        uint16_t value = 0U;

        if (!ui_->get_field(view.page, field.ui_field, value))
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
    if (ui_ == nullptr)
    {
        return;
    }

    if (ui_->page() == Ui::Page::Main)
    {
        switch (action)
        {
            case DwinAction::Start:
                ui_->execute(
                    Ui::Action{
                        Ui::ActionType::StartProfileSelection,
                        0U
                    });
                break;

            case DwinAction::Edit:
                ui_->execute(
                    Ui::Action{
                        Ui::ActionType::EditProfileSelection,
                        0U
                    });
                break;

            case DwinAction::Settings:
                ui_->execute(
                    Ui::Action{
                        Ui::ActionType::Settings,
                        0U
                    });
                break;

            case DwinAction::Events:
                ui_->execute(
                    Ui::Action{
                        Ui::ActionType::ShowEvents,
                        0U
                    });
                break;

            default:
                break;
        }
    }

    if (ui_->page() == Ui::Page::Settings)
    {
        handle_settings_action(action);

        if (action == DwinAction::Home)
        {
            ui_->execute(
                Ui::Action{
                    Ui::ActionType::Back,
                    0U
                });
        }

        return;
    }

    if (ui_->page() == Ui::Page::Monitor)
    {
        switch (action)
        {
            case DwinAction::Stop:
                ui_->execute(
                    Ui::Action{
                        Ui::ActionType::StopFurnace,
                        0U
                    });
                break;

            case DwinAction::Back:
                ui_->execute(
                    Ui::Action{
                        Ui::ActionType::Back,
                        0U
                    });
                break;

            default:
                break;
        }
    }
}

void DwinRenderer::handle_settings_action(DwinAction action) noexcept
{
    switch (action)
    {
        case DwinAction::Previous:
        case DwinAction::Next:
            view_ = (view_ == DwinView::SettingsPid)
                ? DwinView::SettingsOther
                : DwinView::SettingsPid;
            break;
        default:
            break;
    }
}

} // namespace app