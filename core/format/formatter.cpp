#include "formatter.hpp"


namespace core::format
{


void Formatter::clear() noexcept
{
    index_ = 0;
    buffer_[0] = '\0';
}


void Formatter::write(const char* text) noexcept
{
    while (*text)
    {
        if (index_ >= sizeof(buffer_) - 1)
        {
            break;
        }

        buffer_[index_++] = *text++;
    }

    buffer_[index_] = '\0';
}


void Formatter::write(char value) noexcept
{
    if (index_ < sizeof(buffer_) - 1)
    {
        buffer_[index_++] = value;
        buffer_[index_] = '\0';
    }
}


void Formatter::write(bool value) noexcept
{
    write(value ? "true" : "false");
}


void Formatter::write(int32_t value) noexcept
{
    if (value < 0)
    {
        write('-');

        write_unsigned(
            static_cast<uint32_t>(-(value + 1)) + 1);

        return;
    }

    write_unsigned(
        static_cast<uint32_t>(value));
}


void Formatter::write(uint32_t value) noexcept
{
    write_unsigned(value);
}


void Formatter::write_unsigned(
    uint32_t value) noexcept
{
    char digits[10];

    unsigned count = 0;


    do
    {
        digits[count++] =
            static_cast<char>('0' + value % 10);

        value /= 10;

    } while (value != 0);


    while (count > 0)
    {
        write(digits[--count]);
    }
}


const char* Formatter::c_str() const noexcept
{
    return buffer_;
}


}