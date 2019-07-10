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

typedef int8_t PalIdx;
enum {
    PAL_DEFAULT = 0,

    PAL_SNOW = PAL_DEFAULT,
    PAL_AQUA,
    PAL_FIRE,

    PAL_END
};

const Palette *paletteGet(PalIdx index);
PalIdx paletteGetIndex(void);
void paletteSetIndex(PalIdx index);

#endif // PALETTE_H
