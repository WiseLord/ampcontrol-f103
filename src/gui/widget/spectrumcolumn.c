#include "spectrumcolumn.h"

#include "gui/canvas.h"

void spectrumColumnDraw(SpectrumColumn *col, GlcdRect *rect, bool clear, color_t *grad)
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

    y += rect->h;

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
        if (NULL != grad) {
            glcdDrawVertGrad(x, y - s, w, s, grad);
        } else {
            glcdDrawRect(x, y - s, w, s, pal->spColB);
        }
    } else {

        // Draw part of changed column
        if (s > os) {
            if (NULL != grad) {
                glcdDrawVertGrad(x, y - s, w, s - os, &grad[rect->h - s]);
            } else {
                glcdDrawRect(x, y - s, w, s - os, pal->spColB);
            }
        } else if (s < os) {
            glcdDrawRect(x, y - os, w, os - s, pal->bg);
        }

        // Clear old peak
        if (p >= s) {
            glcdDrawRect(x, y - p - 1, w, 1, pal->bg);
        }
    }

    // Draw new peak
    if (p > s) {
        glcdDrawRect(x, y - p, w, 1, pal->spPeak);
    }
}
