#ifndef DISPDEFS_H
#define DISPDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "fonts.h"

#define LCD_ROTATE_0        0x00
#define LCD_ROTATE_180      0x02

#define LCD_BR_MIN          1
#define LCD_BR_MAX          32

typedef struct {
    void (*init)(void);
    void (*sleep)(void);
    void (*wakeup)(void);
    void (*setWindow)(int16_t x, int16_t y, int16_t w, int16_t h);
    void (*rotate)(uint8_t rotate);
    void (*shift)(int16_t value);

    int16_t width;
    int16_t height;
} DispDriver;

#ifdef __cplusplus
}
#endif

#endif // DISPDEFS_H
