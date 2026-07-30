#pragma once

#include <cstdint>

namespace core
{

template<class Backend>
class BasicLog
{
public:

    template<typename... Args>
    static void info(const char* tag, Args&&... args) noexcept
    {
        write("[INFO] [");
        write(tag);
        write("] ");

        write_all(args...);

        write('\n');
    }


    template<typename... Args>
    static void warning(const char* tag, Args&&... args) noexcept
    {
        write("[WARNING] [");
        write(tag);
        write("] ");

        write_all(args...);

        write('\n');
    }


    template<typename... Args>
    static void error(const char* tag, Args&&... args) noexcept
    {
        write("[ERROR] [");
        write(tag);
        write("] ");

        write_all(args...);

        write('\n');
    }


private:

    static void write(char c) noexcept
    {
        Backend::write(c);
    }


    static void write(const char* text) noexcept
    {
        while (*text)
        {
            write(*text++);
        }
    }


    static void write(bool value) noexcept
    {
        write(value ? "true" : "false");
    }


    static void write(uint8_t value) noexcept
    {
        write(static_cast<uint32_t>(value));
    }


    static void write(uint16_t value) noexcept
    {
        write(static_cast<uint32_t>(value));
    }


    static void write(uint32_t value) noexcept
    {
        char buffer[10];
        uint8_t index = 0;

        if (value == 0)
        {
            write('0');
            return;
        }

        while (value > 0)
        {
            buffer[index++] =
                static_cast<char>('0' + (value % 10));

            value /= 10;
        }

        while (index > 0)
        {
            write(buffer[--index]);
        }
    }


    static void write(int8_t value) noexcept
    {
        write(static_cast<int32_t>(value));
    }


    static void write(int16_t value) noexcept
    {
        write(static_cast<int32_t>(value));
    }


    static void write(int32_t value) noexcept
    {
        if (value < 0)
        {
            write('-');

            // avoid overflow for INT32_MIN
            uint32_t magnitude =
                static_cast<uint32_t>(-(value + 1));

            magnitude += 1;

            write(magnitude);
            return;
        }

        write(static_cast<uint32_t>(value));
    }


    template<typename T>
    static void write_all(T&& value) noexcept
    {
        write(value);
    }


    template<typename T, typename... Args>
    static void write_all(T&& value, Args&&... args) noexcept
    {
        write(value);
        write_all(args...);
    }
};

} // namespace core