#include "dwin_renderer.hpp"

namespace app
{

namespace
{

constexpr uint16_t ButtonStop = 0x2000U;
constexpr uint16_t ButtonBack = 0x2001U;
constexpr uint16_t ButtonPrevious = 0x2002U;
constexpr uint16_t ButtonNext = 0x2003U;
constexpr uint16_t ButtonHome = 0x2004U;

struct FieldMapping
{
    uint8_t ui_field;
    uint16_t vp_address;
};

struct PageDescriptor
{
    Ui::Page page;
    DwinView view;
    const FieldMapping* fields;
    std::size_t field_count;
};

constexpr FieldMapping monitor_fields[] = {
    {0U, 0x1000U}, // State
    {1U, 0x1001U}, // Profile
    {2U, 0x1002U}, // Step
    {3U, 0x1003U}, // Step type
    {4U, 0x1004U}, // Temperature
    {5U, 0x1005U}, // Setpoint
    {6U, 0x1006U}, // Step elapsed
    {7U, 0x1007U}, // Profile elapsed
    {8U, 0x1008U}, // Power
    {9U, 0x1009U}, // Outputs
};

constexpr FieldMapping settings_pid_fields[] = {
    {1U, 0x1100U}, // PID Kp
    {2U, 0x1101U}, // PID Ki
    {3U, 0x1102U}, // PID Kd
};

constexpr FieldMapping settings_other_fields[] = {
    {0U, 0x1110U}, // Buzzer
    {4U, 0x1111U}, // Max temperature
    {5U, 0x1112U}, // Prestep outputs
};

constexpr PageDescriptor page_descriptors[] = {
    {
        Ui::Page::Monitor,
        DwinView::Monitor,
        monitor_fields,
        std::size(monitor_fields)
    },
    {
        Ui::Page::Settings,
        DwinView::SettingsPid,
        settings_pid_fields,
        std::size(settings_pid_fields)
    },
    {
        Ui::Page::Settings,
        DwinView::SettingsOther,
        settings_other_fields,
        std::size(settings_other_fields)
    },
};

const PageDescriptor* find_page(
    Ui::Page page,
    DwinView view) noexcept
{
    for (const PageDescriptor& descriptor : page_descriptors)
    {
        if (descriptor.page == page &&
            descriptor.view == view)
        {
            return &descriptor;
        }
    }

    return nullptr;
}

} // namespace

void DwinRenderer::init(
    Ui& ui,
    DwinTransport& transport) noexcept
{
    ui_ = &ui;
    transport_ = &transport;

    rendered_page_ = Ui::Page::Count;

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
            const DwinAction action =
                decode_action(event.address, event.value);

            if (action != DwinAction::None)
            {
                handle_action(action);
            }
        }
    }

    const Ui::Page page = ui_->page();

    if (page != rendered_page_)
    {
        rendered_page_ = page;

        for (std::size_t i = 0; i < MaxRenderedFields; ++i)
        {
            field_rendered_[i] = false;
        }

        render_page(page);
        return;
    }

    render_page(page);
}


void DwinRenderer::render_page(Ui::Page page) noexcept
{
    const PageDescriptor* descriptor =
        find_page(page, view_);

    if (descriptor == nullptr)
    {
        return;
    }

    for (std::size_t i = 0; i < descriptor->field_count; ++i)
    {
        const FieldMapping& field = descriptor->fields[i];

        if (field.ui_field >= MaxRenderedFields)
        {
            continue;
        }

        uint16_t value = 0U;

        if (!ui_->get_field(page, field.ui_field, value))
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

        if (transport_ == nullptr)
        {
            return;
        }

        const DwinProtocol::Packet packet =
            protocol_.write_word(field.vp_address, value);

        transport_->send(packet.data, packet.size);
    }
}

DwinRenderer::DwinAction DwinRenderer::decode_action(
    uint16_t address,
    uint16_t value) const noexcept
{
    (void)value;

    switch (address)
    {
        case ButtonStop:
            return DwinAction::Stop;

        case ButtonBack:
            return DwinAction::Back;

        case ButtonPrevious:
            return DwinAction::Previous;

        case ButtonNext:
            return DwinAction::Next;

        case ButtonHome:
            return DwinAction::Home;

        default:
            return DwinAction::None;
    }
}


void DwinRenderer::handle_action(DwinAction action) noexcept
{
    if (ui_ == nullptr)
    {
        return;
    }

    switch (ui_->page())
    {
        case Ui::Page::Settings:
            handle_settings_action(action);
            break;

        default:
            break;
    }
}


void DwinRenderer::handle_settings_action(
    DwinAction action) noexcept
{
    switch (action)
    {
        case DwinAction::Previous:
        case DwinAction::Next:
            if (view_ == DwinView::SettingsPid)
            {
                view_ = DwinView::SettingsOther;
            }
            else
            {
                view_ = DwinView::SettingsPid;
            }

            rendered_page_ = Ui::Page::Count;
            break;

        default:
            break;
    }
}

} // namespace app