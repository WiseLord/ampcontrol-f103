#include "spectrumcolumn.h"

#include "../canvas.h"

void spectrumColumnDraw(SpectrumColumn *col, GlcdRect *rect, bool clear)
{
    Canvas *canvas = canvasGet();

    int16_t x = rect->x;
    int16_t y = rect->y;
    int16_t w = rect->w;
    int16_t h = rect->h;

    int16_t s = col->showW;
    int16_t os = col->prevW;
    int16_t p = col->peakW;

    const Palette *pal = canvas->pal;

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
    if (clear) {
        glcdDrawRect(x, y + h - s, w, s, pal->spCol);

        if (p > s) {
            glcdDrawRect(x, y + h - p, w, 1, pal->spPeak);
        }
        return;
    }

    // Draw part of changed column
    if (s > os) {
        glcdDrawRect(x, y + h - s, w, s - os, pal->spCol);
    } else if (s < os) {
        glcdDrawRect(x, y + h - os, w, os - s, pal->bg);
    }

    // Clear old peak
    if (p >= s) {
        glcdDrawRect(x, y + h - p - 1, w, 1, pal->bg);
    }

    // Draw new peak
    if (p > s) {
        glcdDrawRect(x, y + h - p, w, 1, pal->spPeak);
    }
}
