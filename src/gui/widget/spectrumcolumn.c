#include "spectrumcolumn.h"

#include <stddef.h>

#include "gui/palette.h"

void spectrumColumnDraw(bool clear, SpectrumColumn *col, GlcdRect *rect, bool mirror, color_t *grad)
{
    int16_t x = rect->x;
    int16_t y = rect->y;
    int16_t w = rect->w;
    int16_t h = rect->h;

    int16_t s = col->showW;
    int16_t os = col->prevW;
    int16_t p = col->peakW;

    const Palette *pal = paletteGet();

    if (s == 0) {
        s = 1;
    }
    if (s >= h) {
        s = h - 1;
    }
    if (p >= h) {
        p = h - 1;
    }
    if (os >= h) {
        os = h - 1;
    }

    // Full redraw the column
    if (0 || clear) {
        glcdDrawVertGrad(x, mirror ? y : y + h - s, w, s, &grad[mirror ? 0 : h - s]);
        glcdDrawRect(x, mirror ? y + s : y, w, h - s, pal->bg);
    } else {

        // Draw part of changed column
        if (s > os) {
            glcdDrawVertGrad(x, mirror ? y + os : y + h - s, w, s - os, &grad[mirror ? os : h - s]);
        } else if (s < os) {
            glcdDrawRect(x, mirror ? y + s : y + h - os, w, os - s, pal->bg);
        }

        // Clear old peak
        if (p >= s) {
            glcdDrawRect(x, mirror ? y + p : y + h - p - 1, w, 1, pal->bg);
        }
    }

    // Draw new peak
    if (p > s) {
        glcdDrawRect(x, mirror ? y + p - 1: y + h - p, w, 1, pal->spPeak);
    }
}
