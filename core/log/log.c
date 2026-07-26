//
// log.c — lightweight embedded logging back-end
//
// All text formatting is delegated to the print module (print.h / print.c).
// This file only handles level filtering, log-line structure, and colour.
//
// Compile-time elimination — the entire file is wrapped in #if LOG_ENABLE
// so it compiles to an empty object when logging is not needed.
//

#include <stdint.h>
#include <stdarg.h>
//#include <string.h>
#include "log.h"
#include "print.h"
#include "utils.h"

#if LOG_ENABLE

static uint32_t get_tag_level(const char *tag);

// ---------------------------------------------------------------
// Module instance
// ---------------------------------------------------------------

struct log_s {
    uint8_t default_level;
    struct log_level_s * tag_map;
    out_char_t out;
    tick_cb_t tick_cb;
    uint8_t tag_map_size;
};

static struct log_s instance;

// ---------------------------------------------------------------
// Public API
// ---------------------------------------------------------------

void log_init(uint8_t def_lev, out_char_t out, tick_cb_t tick,
              struct log_level_s * tag_map, uint8_t tag_map_size)
{
    instance.default_level = def_lev;
    instance.out = out;
    instance.tick_cb = tick;
    instance.tag_map = tag_map;
    instance.tag_map_size = tag_map_size;
}

void log_write(uint32_t level, const char *color, const char *level_str,
               const char *tag, const char *file, uint32_t line,
               const char *fmt, ...)
{
    if (!instance.out) return;

    if (get_tag_level(tag) >= level) {
        print_puts(instance.out, color);
        print_puts(instance.out, "["); print_puts(instance.out, level_str);
        print_puts(instance.out, "] [");
        print_put_dec(instance.out, instance.tick_cb());
        print_puts(instance.out, "] [");
        print_puts(instance.out, tag);
        print_puts(instance.out, "] ");
        if (file) {
            print_puts(instance.out, file);
            print_putc(instance.out, ':');
            print_put_dec(instance.out, line);
            print_puts(instance.out, ": ");
        }
        va_list args;
        va_start(args, fmt);
        vprint_fmt(instance.out, fmt, args);
        va_end(args);
        print_puts(instance.out, COLOR_RESET);
        print_puts(instance.out, "\r\n");
    }
}

void log_write_hex(const char *tag, const char *label,
                   const void *data, uint32_t len)
{
    if (!instance.out) return;

    if (get_tag_level(tag) >= LOG_LEVEL_DEBUG) {
        print_puts(instance.out, COLOR_MAGENTA "[HEX] [");
        print_put_dec(instance.out, instance.tick_cb());
        print_puts(instance.out, "] [");
        print_puts(instance.out, tag);
        print_puts(instance.out, "] ");
        print_puts(instance.out, label);
        print_puts(instance.out, ": ");
        const uint8_t *bytes = (const uint8_t *)data;
        for (uint32_t i = 0; i < len; i++) {
            print_put_hex_byte(instance.out, bytes[i]);
            print_putc(instance.out, ' ');
        }
        print_puts(instance.out, COLOR_RESET);
        print_puts(instance.out, "\r\n");
    }
}

// ---------------------------------------------------------------
// Private
// ---------------------------------------------------------------

uint32_t get_tag_level(const char *tag)
{
    for (uint16_t i = 0; i < instance.tag_map_size; i++)
    {
        if (strcmp(instance.tag_map[i].tag, tag) == 0)
            return instance.tag_map[i].level;
    }
    return instance.default_level;
}

#endif  // LOG_ENABLE
