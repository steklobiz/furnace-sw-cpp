// print.h — universal formatted output through an abstract character callback
//
// Why this exists:
//   printf-family functions pull in tens of KB of formatting machinery
//   and require a FILE-like output sink.  On bare-metal targets (UART,
//   LCD, USB CDC, etc.) you typically only need to send characters one
//   at a time through a simple register write.
//
//   This module provides the same formatting (%s %d %u %x %X %%) with
//   zero global state, zero buffers, and zero external dependencies
//   beyond <stdint.h> and <stdarg.h>.  The caller supplies an out_char_t
//   callback and the library does the rest.
//
// Two entry points:
//   print_fmt — variadic, for callers with concrete arguments
//   vprint_fmt — va_list-based, for wrapping inside other variadic functions
//                (used by log_write in the log module)
//
// Supported specifiers:  %s %d %u %x %X %%
//   Width, precision, padding, length modifiers and floating-point are
//   intentionally not supported.  Adding them would reintroduce the very
//   complexity this module is designed to avoid.
//
// Thread safety:
//   None.  The module holds no state, so safety depends entirely on
//   the caller's out_char_t callback.  If that callback is not
//   reentrant, calls from multiple contexts must be serialised.

#ifndef PRINT_H
#define PRINT_H

#include <stdint.h>
#include <stdarg.h>

// Callback for emitting a single character.
// In bare-metal code this is typically a UART TX register write.
// The callback is called once per character — no buffering occurs here.
typedef void (*out_char_t)(char c);

void print_fmt(out_char_t out, const char *fmt, ...);
void vprint_fmt(out_char_t out, const char *fmt, va_list args);
void print_puts(out_char_t out, const char *s);
void print_putc(out_char_t out, char c);
void print_put_dec(out_char_t out, uint32_t n);
void print_put_hex(out_char_t out, uint32_t n, int upper);
void print_put_hex_byte(out_char_t out, uint8_t b);

#endif
