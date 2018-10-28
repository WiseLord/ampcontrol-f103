#ifndef KS0108_H
#define KS0108_H

#include <stdint.h>
#include "../dispdrv.h"

// Display geometry
#define KS0108_COLS                 64
#define KS0108_ROWS                 8
#define KS0108_CHIPS                2

// Display control instructions
#define KS0108_DISPLAY_ON           0x3F
#define KS0108_DISPLAY_OFF          0x3E

#define KS0108_SET_ADDRESS          0x40
#define KS0108_SET_PAGE             0xB8
#define KS0108_DISPLAY_START_LINE   0xC0

// Status bytes
#define KS0108_STA_RESET            0x10 // DB7
#define KS0108_STA_DISP_OFF         0x20 // DB5
#define KS0108_STA_BUSY             0x80 // DB4

// Some common defines
#define KS0108_COMMAND              0
#define KS0108_DATA                 1

#define KS0108_PHASE_SET_PAGE       64
#define KS0108_PHASE_SET_ADDR       65

void ks0108Init(DispDriver **driver);
void ks0108Clear(void);
void ks0108IRQ(void);

void ks0108DrawPixel(int16_t x, int16_t y, uint16_t color);

#endif // KS0108_H
