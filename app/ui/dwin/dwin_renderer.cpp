#include "dwin_renderer.hpp"

namespace app
{

namespace
{

struct Field
{
    uint8_t ui_field;
    uint16_t address;
};

struct PageDescriptor
{
    Ui::Page page;
    const Field* fields;
    std::size_t field_count;
};

constexpr Field monitor_fields[] = {
    {0U, 0x1000U},
    {1U, 0x1001U},
    {2U, 0x1002U},
    {3U, 0x1003U},
};

constexpr Field settings_fields[] = {
    {0U, 0x1100U},
    {1U, 0x1101U},
    {2U, 0x1102U},
    {3U, 0x1103U},
};

constexpr PageDescriptor page_descriptors[] = {
    {
        Ui::Page::Monitor,
        monitor_fields,
        sizeof(monitor_fields) / sizeof(monitor_fields[0])
    },
    {
        Ui::Page::Settings,
        settings_fields,
        sizeof(settings_fields) / sizeof(settings_fields[0])
    },
};

const PageDescriptor* find_page(Ui::Page page) noexcept
{
    for (const PageDescriptor& descriptor : page_descriptors)
    {
        if (descriptor.page == page)
        {
            return &descriptor;
        }
    }

    return nullptr;
}

} // namespace

void DwinRenderer::init(Ui& ui) noexcept
{
    ui_ = &ui;
    rendered_page_ = Ui::Page::Count;

    for (std::size_t i = 0; i < MaxFieldsPerPage; ++i)
    {
        rendered_values_[i] = 0U;
        field_rendered_[i] = false;
    }
}

void DwinRenderer::process() noexcept
{
    if (ui_ == nullptr)
    {
        return;
    }

    const Ui::Page page = ui_->page();

    if (page != rendered_page_)
    {
        rendered_page_ = page;

        for (std::size_t i = 0; i < MaxFieldsPerPage; ++i)
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
    const PageDescriptor* descriptor = find_page(page);

    if (descriptor == nullptr)
    {
        return;
    }

    for (std::size_t i = 0; i < descriptor->field_count; ++i)
    {
        const Field& field = descriptor->fields[i];

        if (field.ui_field >= MaxFieldsPerPage)
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

        const DwinProtocol::Packet packet =
            protocol_.write_word(field.address, value);

        (void)packet;
    }
}

} // namespace app