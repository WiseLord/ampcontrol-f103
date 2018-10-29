#ifndef DISPDRV_H
#define DISPDRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "fonts/fonts.h"

typedef struct {
    void (*drawPixel)(int16_t x, int16_t y, uint16_t color);
    void (*drawRectangle)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void (*drawImage)(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);
    void (*updateFB)(void);

    uint16_t width;
    uint16_t height;

    uint8_t bus;
} DispDriver;

#define LCD_BR_MIN                      0
#define LCD_BR_MAX                      32

#if defined (_KS0108A) || defined(_KS0108B)
#include "gm128x64/ks0108.h"
#elif defined (_ST7920)
#include "gm128x64/st7920.h"
#elif defined (_SSD1306)
#include "gm128x64/ssd1306.h"
#elif defined (_ILI9163)
#include "gc160x128/ili9163.h"
#elif defined (_ST7735)
#include "gc160x128/st7735.h"
#elif defined (_LS020)
#include "gc176x132/ls020.h"
#elif defined (_LPH9157)
#include "gc176x132/lph9157.h"
#elif defined (_SSD1286A)
#include "gc176x132/ssd1286a.h"
#elif defined (_HX8340)
#include "gc220x176/hx8340.h"
#elif defined (_ILI9225)
#include "gc220x176/ili9225.h"
#elif defined (_ILI9320)
#include "gc320x240/ili9320.h"
#elif defined (_ILI9341)
#include "gc320x240/ili9341.h"
#elif defined (_S6D0139)
#include "gc320x240/s6d0139.h"
#elif defined (_SPFD5408)
#include "gc320x240/spfd5408.h"
#elif defined (_MC2PA8201)
#include "gc320x240/mc2pa8201.h"
#elif defined (_ILI9327)
#include "gc400x240/ili9327.h"
#elif defined (_ST7793)
#include "gc400x240/st7793.h"
#elif defined (_ILI9481)
#include "gc480x320/ili9481.h"
#elif defined (_R61581)
#include "gc480x320/r61581.h"
#else
#ifndef EMUL_DISP
#error "Unsupported display driver"
#endif
#endif

#if defined(_KS0108A) || defined(_KS0108B)
#define DISPLAY_IRQ ks0108IRQ
#elif defined(_ST7920)
#define DISPLAY_IRQ st7920IRQ
#else
#define DISPLAY_IRQ dispdrvBusIRQ
#endif

void dispdrvInit(DispDriver **glcd);
void dispdrvPwm(void);
void dispdrvSetBrightness(uint8_t value);
uint8_t dispdrvGetBus(void);

void dispdrvBusIRQ(void);

void dispdrvWaitOperation(void);
void dispdrvSendData8(uint8_t data);
void dispdrvSendData16(uint16_t data);
void dispdrvSendFill(uint32_t size, uint16_t color);
void dispdrvSendImage(tImage *img, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // DISPDRV_H
