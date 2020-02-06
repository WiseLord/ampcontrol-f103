#ifndef COLORS_H
#define COLORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef _SSD1322
typedef uint8_t color_t;
#else
typedef uint16_t color_t;
#endif

//Colors
#define RGB_R(x)                    (((x) >> 16) & 0xFF)
#define RGB_G(x)                    (((x) >> 8) & 0xFF)
#define RGB_B(x)                    (((x) >> 0) & 0xFF)

// RGB_CONV
#ifdef _SSD1322
#define RGB_CONV(x)                 ((color_t)(((RGB_R(x) >> 4) + \
                                                (RGB_G(x) >> 4) + \
                                                (RGB_B(x) >> 4)) / 3))
#define COLOR_GRAY16(x)             ((color_t)(x & 0x0F))
#else

#define RGB_CONV(x)                 ((color_t)(((RGB_R(x) & 0xF8) << 8) | \
                                               ((RGB_G(x) & 0xFC) << 3) | \
                                               ((RGB_B(x) & 0xF8) >> 3)))
#define COLOR_GRAY16(x)             ((color_t)RGB_CONV(((x) << 0)  | \
                                                       ((x) << 4)  | \
                                                       ((x) << 8)  | \
                                                       ((x) << 12) | \
                                                       ((x) << 16) | \
                                                       ((x) << 20)))
#endif

#define COLOR_BLACK                 RGB_CONV(0x000000)
#define COLOR_NAVI                  RGB_CONV(0x000080)
#define COLOR_BLUE                  RGB_CONV(0x0000FF)
#define COLOR_GREEN                 RGB_CONV(0x008000)
#define COLOR_TEAL                  RGB_CONV(0x008080)
#define COLOR_DODGER_BLUE           RGB_CONV(0x0080FF)
#define COLOR_LIME                  RGB_CONV(0x00FF00)
#define COLOR_SPRING_GREEN          RGB_CONV(0x00FF80)
#define COLOR_AQUA                  RGB_CONV(0x00FFFF)
#define COLOR_NERO                  RGB_CONV(0x252525)
#define COLOR_ECLIPSE               RGB_CONV(0x3F3939)
#define COLOR_MAROON                RGB_CONV(0x800000)
#define COLOR_PURPLE                RGB_CONV(0x800080)
#define COLOR_ELECTRIC_INDIGO       RGB_CONV(0x8000FF)
#define COLOR_OLIVE                 RGB_CONV(0x808000)
#define COLOR_GRAY                  RGB_CONV(0x808080)
#define COLOR_LIGHT_SLATE_BLUE      RGB_CONV(0x8080FF)
#define COLOR_CHARTREUSE            RGB_CONV(0x80FF00)
#define COLOR_LIGHT_GREEN           RGB_CONV(0x80FF80)
#define COLOR_ELECTRIC_BLUE         RGB_CONV(0x80FFFF)
#define COLOR_RED                   RGB_CONV(0xFF0000)
#define COLOR_DEEP_PINK             RGB_CONV(0xFF0080)
#define COLOR_MAGENTA               RGB_CONV(0xFF00FF)
#define COLOR_DARK_ORANGE           RGB_CONV(0xFF8000)
#define COLOR_LIGHT_CORAL           RGB_CONV(0xFF8080)
#define COLOR_FUCHSIA_PINK          RGB_CONV(0xFF80FF)
#define COLOR_YELLOW                RGB_CONV(0xFFFF00)
#define COLOR_WITCH_HAZE            RGB_CONV(0xFFFF80)
#define COLOR_WHITE                 RGB_CONV(0xFFFFFF)

#ifdef __cplusplus
}
#endif

#endif // COLORS_H
