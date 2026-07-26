//
// print.c — char-by-char formatted output, no stdio dependency
//
// Uses a reverse-fill technique for integer-to-ASCII conversion:
// we fill a small buffer from the end towards the start, then
// emit it forward.  This avoids needing to know the number of
// digits before converting.
//
// The 12-byte scratch buffer is sufficient for 32-bit values:
//   %d → "-2147483648\0"  (12 chars)
//   %x → "FFFFFFFF\0"     (9 chars)
//
// All functions take an explicit out_char_t callback — no global
// state, so there is nothing to initialise or tear down.
//

#include <stdint.h>
#include <stdarg.h>
#include "print.h"

// ---------------------------------------------------------------
// Low-level helpers
// ---------------------------------------------------------------

void print_puts(out_char_t out, const char *s)
{
    if (!out || !s) return;
    while (*s) out(*s++);
}

void print_putc(out_char_t out, char c)
{
    if (!out) return;
    out(c);
}

// Reverse-fill decimal ASCII (end of buf → start), then emit forward.
void print_put_dec(out_char_t out, uint32_t n)
{
    if (!out) return;
    char buf[12];
    char *p = buf + sizeof(buf);
    *--p = '\0';
    do { *--p = '0' + n % 10; } while (n /= 10);
    print_puts(out, p);
}

// Reverse-fill hexadecimal ASCII, then emit forward.
// upper: 0 → lowercase hex digits, non-zero → uppercase.
void print_put_hex(out_char_t out, uint32_t n, int upper)
{
    if (!out) return;
    const char *hex = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    char buf[12];
    char *p = buf + sizeof(buf);
    *--p = '\0';
    if (n == 0) {
        *--p = '0';
    } else {
        while (n) {
            *--p = hex[n & 0x0F];
            n >>= 4;
        }
    }
    print_puts(out, p);
}

// Always emits exactly 2 characters (e.g. "0A", "FF").
void print_put_hex_byte(out_char_t out, uint8_t b)
{
    if (!out) return;
    const char *hex = "0123456789ABCDEF";
    out(hex[b >> 4]);
    out(hex[b & 0x0F]);
}

// ---------------------------------------------------------------
// Formatter — parses fmt, calls helpers for each specifier
// ---------------------------------------------------------------

// va_list variant — used by log.c (and any other wrapper that
// already has a va_list and doesn't want to re-walk it).
void vprint_fmt(out_char_t out, const char *fmt, va_list args)
{
    if (!out || !fmt) return;
    while (*fmt) {
        if (*fmt != '%') {
            out(*fmt++);
            continue;
        }
        fmt++;
        // Trailing '%' at end of string — just emit it and stop.
        if (*fmt == '\0') {
            out('%');
            break;
        }
        switch (*fmt) {
            case 's': {
                const char *s = va_arg(args, const char *);
                print_puts(out, s);
                break;
            }
            case 'd': {
                int v = va_arg(args, int);
                if (v < 0) { out('-'); v = -v; }
                print_put_dec(out, (uint32_t)v);
                break;
            }
            case 'u': {
                uint32_t v = va_arg(args, uint32_t);
                print_put_dec(out, v);
                break;
            }
            case 'x':
            case 'X': {
                uint32_t v = va_arg(args, uint32_t);
                print_put_hex(out, v, *fmt == 'X');
                break;
            }
            case '%':
                out('%');
                break;
            default:
                // Unknown specifier — pass through literally.
                out('%');
                out(*fmt);
                break;
        }
        fmt++;
    }
}

// Variadic wrapper — for direct calls from application code.
void print_fmt(out_char_t out, const char *fmt, ...)
{
    if (!out || !fmt) return;
    va_list args;
    va_start(args, fmt);
    vprint_fmt(out, fmt, args);
    va_end(args);
}
