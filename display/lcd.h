#ifndef LCD_H
#define LCD_H

#include <inttypes.h>

#if defined (_KS0108)
#include "ks0108.h"
#elif defined (_ST7920)
#include "st7920.h"
#elif defined (_SSD1306)
#include "ssd1306.h"
#elif defined (_LS020)
#include "ls020.h"
#elif defined (_LPH9157)
#include "lph9157.h"
#elif defined (_SSD1286A)
#include "ssd1286a.h"
#elif defined (_ILI9320)
#include "ili9320.h"
#elif defined (_ILI9341)
#include "ili9341.h"
#elif defined (_S6D0139)
#include "s6d0139.h"
#elif defined (_SPFD5408)
#include "spfd5408.h"
#else
#error "Unsupported display driver"
#endif

#if defined(_KS0108)
#define LCD_IRQ ks0108IRQ
#elif defined(_ST7920)
#define LCD_IRQ st7920IRQ
#elif defined(_ILI9320)
#define LCD_IRQ ili9320BusIRQ
#elif defined(_S6D0139)
#define LCD_IRQ s6d0139BusIRQ
#elif defined(_SPFD5408)
#define LCD_IRQ spfd5408BusIRQ
#else
#define LCD_IRQ void
#endif

void lcdInit(GlcdDriver **glcd);
void lcdPWM(void);
void lcdSetBrightness(uint8_t value);
uint8_t lcdGetInput(void);

#endif // LCD_H
