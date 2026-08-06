#pragma once

#include <cstddef>
#include <cstdint>


namespace core::format
{

class Formatter
{
public:

    Formatter() noexcept = default;


    void clear() noexcept;


    void write(const char* text) noexcept;

    void write(char value) noexcept;

    void write(bool value) noexcept;

    void write(int32_t value) noexcept;

    void write(uint32_t value) noexcept;


    const char* c_str() const noexcept;


private:

    void write_unsigned(uint32_t value) noexcept;


private:

    char buffer_[96]{};
    std::size_t index_ = 0;
};

}