#ifndef DISPLAY_H
#define DISPLAY_H

#include <inttypes.h>

#if defined (_KS0108)
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
#else
#error "Unsupported display driver"
#endif

#if defined(_KS0108)
#define DISPLAY_IRQ ks0108IRQ
#elif defined(_ST7920)
#define DISPLAY_IRQ st7920IRQ
#elif defined(_ILI9163)
#define DISPLAY_IRQ ili9163BusIRQ
#elif defined(_ST7735)
#define DISPLAY_IRQ st7735BusIRQ
#elif defined(_HX8340)
#define DISPLAY_IRQ hx8340BusIRQ
#elif defined(_ILI9320)
#define DISPLAY_IRQ ili9320BusIRQ
#elif defined(_S6D0139)
#define DISPLAY_IRQ s6d0139BusIRQ
#elif defined(_SPFD5408)
#define DISPLAY_IRQ spfd5408BusIRQ
#elif defined(_MC2PA8201)
#define DISPLAY_IRQ mc2pa8201BusIRQ
#elif defined(_ILI9327)
#define DISPLAY_IRQ ili9327BusIRQ
#elif defined(_ST7793)
#define DISPLAY_IRQ st7793BusIRQ
#else
#define DISPLAY_IRQ()
#endif

void displayInit(GlcdDriver **glcd);
void displayPWM(void);
void displaySetBrightness(uint8_t value);
uint8_t displayGetInput(void);

#endif // DISPLAY_H
