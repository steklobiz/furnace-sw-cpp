#pragma once

#include <cstdint>
#include <cstddef>

#include "formatter.hpp"
#include "log_output.hpp"


namespace core::log
{

enum class Level : uint8_t
{
    None = 0,
    Error,
    Warning,
    Info
};


struct Tag
{
    const char* name;
    Level max_level;
};


struct Message
{
    Level level;
    const Tag* tag;
    char text[96];
};


namespace detail
{

template<typename... Args>
void output(
    Level level,
    const Tag& tag,
    Args... args) noexcept
{
    if (tag.max_level == Level::None ||
        level > tag.max_level)
    {
        return;
    }


    core::format::Formatter formatter;

    (formatter.write(args), ...);


    Message message{};

    message.level = level;
    message.tag = &tag;


    const char* p = formatter.c_str();
    std::size_t index = 0;

    while (p[index] && index < sizeof(message.text) - 1)
    {
        message.text[index] = p[index];
        ++index;
    }

    message.text[index] = '\0';


    platform::log_output(message);
}

}


template<typename... Args>
void error(
    const Tag& tag,
    Args... args) noexcept
{
    detail::output(
        Level::Error,
        tag,
        args...);
}


template<typename... Args>
void warning(
    const Tag& tag,
    Args... args) noexcept
{
    detail::output(
        Level::Warning,
        tag,
        args...);
}


template<typename... Args>
void info(
    const Tag& tag,
    Args... args) noexcept
{
    detail::output(
        Level::Info,
        tag,
        args...);
}

}