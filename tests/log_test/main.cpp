#include <iostream>

#include "log.hpp"


struct Backend
{
    static void write(char c) noexcept
    {
        std::cout.put(c);
    }
};


using Log = core::log::Logger<Backend>;


int main()
{
    constexpr core::log::Tag tag_off   {"OFF",   core::log::Level::Off};
    constexpr core::log::Tag tag_err   {"ERR",   core::log::Level::Error};
    constexpr core::log::Tag tag_warn  {"WARN",  core::log::Level::Warning};
    constexpr core::log::Tag tag_info  {"INFO",  core::log::Level::Info};

    Log::info(tag_info, "hello");
    Log::warning(tag_warn, "warn msg");
    Log::error(tag_err, "err msg");

    Log::info(tag_warn, "suppressed");
    Log::info(tag_err, "suppressed");
    Log::warning(tag_err, "suppressed");
    Log::error(tag_off, "suppressed");
   
    Log::error(tag_info, "err on info tag");
    Log::warning(tag_info, "warn on info tag");
    Log::error(tag_warn, "err on warn tag");

    Log::info(tag_info, "a=", 1, " b=", true);
    Log::info(tag_info, uint32_t(0));
    Log::info(tag_info, uint32_t(4294967295u));

    Log::info(tag_info, uint8_t(255));
    Log::info(tag_info, int32_t(0));
    Log::info(tag_info, int32_t(42));
    Log::info(tag_info, int32_t(-1));
    Log::info(tag_info, int32_t(-2147483647 - 1));
    Log::info(tag_info, int8_t(-128));
    Log::info(tag_info, true, false);
    Log::info(tag_info, 'A', ' ', 'Z');

    // --- Empty tag name ---
    constexpr core::log::Tag empty_tag {"", core::log::Level::Info};
    Log::info(empty_tag, "x");
    Log::info(tag_info);

    return 0;
}
