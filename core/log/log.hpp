#pragma once

#include <cstdint>
#include <type_traits>

namespace core::log
{

enum class Level : uint8_t
{
    Off,
    Error,
    Warning,
    Info
};

struct Tag
{
    const char* name;
    Level level;
};

template<class Backend>
class Logger
{
public:
    template<class... Args>
    static void info(const Tag& tag, Args&&... args) noexcept
    {
        log(Level::Info, tag, static_cast<Args&&>(args)...);
    }

    template<class... Args>
    static void warning(const Tag& tag, Args&&... args) noexcept
    {
        log(Level::Warning, tag, static_cast<Args&&>(args)...);
    }

    template<class... Args>
    static void error(const Tag& tag, Args&&... args) noexcept
    {
        log(Level::Error, tag, static_cast<Args&&>(args)...);
    }

private:
    template<class... Args>
    static void log(Level level, const Tag& tag, Args&&... args) noexcept
    {
        if (level > tag.level)
            return;

        write_prefix(level);
        write('[');
        write_string(tag.name);
        write(']');
        write(' ');

        (write_value(static_cast<Args&&>(args)), ...);

        write('\n');
    }

    static void write_prefix(Level level) noexcept
    {
        switch(level)
        {
        case Level::Info:    write_string("[INFO] "); break;
        case Level::Warning: write_string("[WARNING] "); break;
        case Level::Error:   write_string("[ERROR] "); break;
        default: break;
        }
    }

    static void write(char c) noexcept { Backend::write(c); }

    static void write_string(const char* s) noexcept
    {
        while(*s) write(*s++);
    }

    static void write_value(const char* s) noexcept { write_string(s); }
    static void write_value(char c) noexcept { write(c); }
    static void write_value(bool v) noexcept { write_string(v ? "true":"false"); }

    template<class T>
    static std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>
    write_value(T v) noexcept
    {
        using U = std::make_unsigned_t<T>;
        if(v<0){ write('-'); write_unsigned(static_cast<U>(-(v+1))+1); }
        else write_unsigned(static_cast<U>(v));
    }

    template<class T>
    static std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>>
    write_value(T v) noexcept
    {
        write_unsigned(v);
    }

    template<class U>
    static void write_unsigned(U value) noexcept
    {
        char buf[10*sizeof(U)];
        unsigned i=0;
        do{
            buf[i++] = char('0' + value%10);
            value/=10;
        }while(value);
        while(i) write(buf[--i]);
    }
};

} // namespace core::log