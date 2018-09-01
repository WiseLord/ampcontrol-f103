#ifndef DISPLAY_H
#define DISPLAY_H

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
#elif defined (_ILI9327)
#include "ili9327.h"
#elif defined (_ST7793)
#include "st7793.h"
#else
#error "Unsupported display driver"
#endif

#if defined(_KS0108)
#define DISPLAY_IRQ ks0108IRQ
#elif defined(_ST7920)
#define DISPLAY_IRQ st7920IRQ
#elif defined(_ILI9320)
#define DISPLAY_IRQ ili9320BusIRQ
#elif defined(_S6D0139)
#define DISPLAY_IRQ s6d0139BusIRQ
#elif defined(_SPFD5408)
#define DISPLAY_IRQ spfd5408BusIRQ
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
