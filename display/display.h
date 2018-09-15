#ifndef DISPLAY_H
#define DISPLAY_H

#include <inttypes.h>

#include "dispdrv.h"

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
#elif defined(_DISP_8BIT)
#define DISPLAY_IRQ dispdrvBusIRQ
#else
#define DISPLAY_IRQ()
#endif

void displayInit(GlcdDriver **glcd);
void displayPWM(void);
void displaySetBrightness(uint8_t value);
uint8_t displayGetInput(void);

#endif // DISPLAY_H
