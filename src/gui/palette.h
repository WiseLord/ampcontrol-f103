#ifndef PALETTE_H
#define PALETTE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "display/glcd.h"

typedef struct {
    color_t fg;
    color_t bg;
    color_t active;
    color_t inactive;
    color_t selected;
    color_t spColB;
    color_t spColG;
    color_t spPeak;
} Palette;

typedef int8_t PalIdx;
enum {
    PAL_DEFAULT = 0,

    PAL_SNOW = PAL_DEFAULT,
    PAL_AQUA,
    PAL_FIRE,

    PAL_END
};

const Palette *paletteGet(void);
PalIdx paletteGetIndex(void);
void paletteSetIndex(PalIdx index);

#ifdef __cplusplus
}
#endif

#endif // PALETTE_H
