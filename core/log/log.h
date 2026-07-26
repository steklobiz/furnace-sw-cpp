// log.h - Universal logging system
//
// Architecture:
//   Single module-encapsulated instance (no true singleton pattern).
//   log.c owns a static struct instance, exposed only through
//   functions declared here.  All output goes char-by-char through
//   a user-provided out_char_t callback — no printf, no buffers,
//   no heap.
//
// Compile-time elimination:
//   #define LOG_ENABLE 0  before including this header to strip
//   every logging macro + the entire log.c translation unit.
//   Zero code, zero data, zero strings in the final binary.
//
// Tag-map level resolution:
//   Each log call passes a tag (string).  log.c looks it up in the
//   user-supplied tag_map[] array.  If the tag is not found, the
//   instance's default_level is used.
//
// Supported format specifiers (log_write fmt argument):
//   %s  null-terminated string
//   %d  signed int
//   %u  unsigned int
//   %x  unsigned int printed in lowercase hex
//   %X  unsigned int printed in UPPERCASE hex
//   %%  literal '%'
//   All other specifiers produce the raw characters (e.g. %f → "%f").
//   No width, precision, or length modifiers are supported.

#ifndef LOG_H
#define LOG_H

#include <stdint.h>
#include <stddef.h> // for NULL

#ifdef __cplusplus
extern "C" {
#endif
#include "print.h"

// ============================================
// Configuration
// ============================================
#define LOG_LEVEL_NONE  0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_DEBUG 4

// Master switch — 0 strips all logging code at compile time.
#ifndef LOG_ENABLE
#define LOG_ENABLE 1
#endif

// Enable/disable ANSI colour escape codes in the output.
#define LOG_USE_COLORS 0

// ============================================
// Colour definitions
// ============================================
#if LOG_USE_COLORS
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_RESET   "\033[0m"
#else
#define COLOR_RED     ""
#define COLOR_YELLOW  ""
#define COLOR_GREEN   ""
#define COLOR_CYAN    ""
#define COLOR_MAGENTA ""
#define COLOR_RESET   ""
#endif

// ============================================
// Callback types
// ============================================

// Tick callback — return current millisecond counter.
// The output callback (out_char_t) is now provided by print.h.
typedef uint32_t (*tick_cb_t)(void);

// ============================================
// Tag-map entry
// ============================================
// Each application module can have a dedicated level threshold.
// The user supplies an array of these to log_init().
struct log_level_s{
    const char* tag;
    uint32_t level;
};

// ============================================
// Public API declarations
// ============================================

void log_write(uint32_t level, const char *color, const char *level_str,
               const char *tag, const char *file, uint32_t line,
               const char *fmt, ...);

void log_write_hex(const char *tag, const char *label,
                   const void *data, uint32_t len);

void log_init(uint8_t def_lev, out_char_t out, tick_cb_t tick,
              struct log_level_s * tag_map, uint8_t tag_map_size);

// ============================================
// Public macros  (use these in application code)
// ============================================

#if LOG_ENABLE

#define LOG_ERROR(tag, fmt, ...) \
    log_write(LOG_LEVEL_ERROR, COLOR_RED, "ERROR", tag, NULL, 0, fmt, ##__VA_ARGS__)

#define LOG_WARN(tag, fmt, ...)  \
    log_write(LOG_LEVEL_WARN, COLOR_YELLOW, "WARN", tag, NULL, 0, fmt, ##__VA_ARGS__)

#define LOG_INFO(tag, fmt, ...)  \
    log_write(LOG_LEVEL_INFO, COLOR_GREEN, "INFO", tag, NULL, 0, fmt, ##__VA_ARGS__)

#define LOG_DEBUG(tag, fmt, ...) \
    log_write(LOG_LEVEL_DEBUG, COLOR_CYAN, "DEBUG", tag, NULL, 0, fmt, ##__VA_ARGS__)

#define LOG_HEX(tag, label, data, len) \
    log_write_hex(tag, label, data, len)

// __BKPT is ARM-specific.  Provide a no-op fallback for host builds.
#ifndef __BKPT
#define __BKPT(x) ((void)(x))
#endif

#define LOG_ASSERT(cond, tag, fmt, ...)                                      \
  do {                                                                       \
    if (!(cond)) {                                                           \
      log_write(LOG_LEVEL_ERROR, COLOR_RED, "ASSERT", tag,                   \
                __FILE__, (uint32_t)__LINE__, fmt, ##__VA_ARGS__);           \
      __BKPT(0);                                                             \
      while (1);                                                             \
    }                                                                        \
  } while (0)

#else   /* LOG_ENABLE == 0 — all macros evaporate */

#define LOG_ERROR(...)      ((void)0)
#define LOG_WARN(...)       ((void)0)
#define LOG_INFO(...)       ((void)0)
#define LOG_DEBUG(...)      ((void)0)
#define LOG_HEX(...)        ((void)0)
#define LOG_ASSERT(cond, ...) ((void)0)

#endif

#ifdef __cplusplus
}
#endif

#endif // LOG_H