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
#define COLOR_GRAY16(x)             ((color_t)RGB_CONV((((uint32_t)x) << 0)  | \
                                                       (((uint32_t)x) << 4)  | \
                                                       (((uint32_t)x) << 8)  | \
                                                       (((uint32_t)x) << 12) | \
                                                       (((uint32_t)x) << 16) | \
                                                       (((uint32_t)x) << 20)))
#endif

#define COLOR_BLACK                 RGB_CONV(0x000000UL)
#define COLOR_NAVI                  RGB_CONV(0x000080UL)
#define COLOR_BLUE                  RGB_CONV(0x0000FFUL)
#define COLOR_GREEN                 RGB_CONV(0x008000UL)
#define COLOR_TEAL                  RGB_CONV(0x008080UL)
#define COLOR_DODGER_BLUE           RGB_CONV(0x0080FFUL)
#define COLOR_LIME                  RGB_CONV(0x00FF00UL)
#define COLOR_SPRING_GREEN          RGB_CONV(0x00FF80UL)
#define COLOR_AQUA                  RGB_CONV(0x00FFFFUL)
#define COLOR_NERO                  RGB_CONV(0x252525UL)
#define COLOR_ECLIPSE               RGB_CONV(0x3F3939UL)
#define COLOR_MAROON                RGB_CONV(0x800000UL)
#define COLOR_PURPLE                RGB_CONV(0x800080UL)
#define COLOR_ELECTRIC_INDIGO       RGB_CONV(0x8000FFUL)
#define COLOR_OLIVE                 RGB_CONV(0x808000UL)
#define COLOR_GRAY                  RGB_CONV(0x808080UL)
#define COLOR_LIGHT_SLATE_BLUE      RGB_CONV(0x8080FFUL)
#define COLOR_CHARTREUSE            RGB_CONV(0x80FF00UL)
#define COLOR_LIGHT_GREEN           RGB_CONV(0x80FF80UL)
#define COLOR_ELECTRIC_BLUE         RGB_CONV(0x80FFFFUL)
#define COLOR_RED                   RGB_CONV(0xFF0000UL)
#define COLOR_DEEP_PINK             RGB_CONV(0xFF0080UL)
#define COLOR_MAGENTA               RGB_CONV(0xFF00FFUL)
#define COLOR_DARK_ORANGE           RGB_CONV(0xFF8000UL)
#define COLOR_LIGHT_CORAL           RGB_CONV(0xFF8080UL)
#define COLOR_FUCHSIA_PINK          RGB_CONV(0xFF80FFUL)
#define COLOR_YELLOW                RGB_CONV(0xFFFF00UL)
#define COLOR_WITCH_HAZE            RGB_CONV(0xFFFF80UL)
#define COLOR_WHITE                 RGB_CONV(0xFFFFFFUL)

#ifdef __cplusplus
}
#endif

#endif // COLORS_H
