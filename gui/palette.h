#ifndef PALETTE_H
#define PALETTE_H

#include <stdint.h>

typedef struct {
    uint16_t fg;
    uint16_t bg;
    uint16_t active;
    uint16_t inactive;
    uint16_t selected;
    uint16_t spCol;
    uint16_t spPeak;
} Palette;

const Palette *paletteGet(void);

#endif // PALETTE_H
