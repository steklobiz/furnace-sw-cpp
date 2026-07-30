#include <iostream>
#include <string>

#include "log.hpp"


struct NoneBackend
{
    static constexpr void write(char) noexcept
    {
    }
};

struct TestBackend
{
    static void write(char c) noexcept
    {
        buffer += c;
    }

    static std::string buffer;
};


std::string TestBackend::buffer;


using Log = core::BasicLog<TestBackend>;


int main()
{
    Log::info(
        "FURNACE",
        "Temperature=",
        uint32_t(25),
        " C"
    );

    Log::warning(
        "APP",
        "Ready=",
        true
    );

    Log::error(
        "MAX6675",
        "Error code=",
        int32_t(-12)
    );

    Log::info("TEST", "zero=", uint32_t(0));

    Log::info("TEST", "max=",
          uint32_t(4294967295));

    Log::info("TEST",
          "negative=",
          int32_t(-2147483648));

    std::cout << TestBackend::buffer;

    return 0;
}