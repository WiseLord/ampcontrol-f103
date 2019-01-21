#ifndef DISPDEFS_H
#define DISPDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "fonts/fonts.h"

#define LCD_ROTATE_0        0x00
#define LCD_ROTATE_180      0x02

#define LCD_BR_MIN          1
#define LCD_BR_MAX          32

typedef struct {
    void (*drawPixel)(int16_t x, int16_t y, uint16_t color);
    void (*drawRectangle)(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void (*drawImage)(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);
    void (*rotate)(uint8_t rotate);
    void (*shift)(int16_t value);

    uint16_t width;
    uint16_t height;

    uint8_t bus;
} DispDriver;

#ifdef __cplusplus
}
#endif

#endif // DISPDEFS_H
