#include "stripedbar.h"

#include "../canvas.h"

void stripedBarDraw(StripedBar *bar, LayoutStripedBar *lt, bool clear)
{
    (void)clear;

    Canvas *canvas = canvasGet();

    const int16_t sc = lt->sc;         // Scale count
    const uint8_t sw = lt->sw;         // Scale width
    const int16_t barPos = lt->barY;
    const uint8_t barHalf = lt->half;
    const uint8_t barMiddle = lt->middle;
    const int16_t width = lt->barW;

    int16_t value = bar->value;
    int16_t min = bar->min;
    int16_t max = bar->max;

    if (min + max) { // Non-symmectic scale => rescale to 0..sc
        value = sc * (value - min) / (max - min);
    } else { // Symmetric scale => rescale to -sc/2..sc/2
        value = (sc / 2) * value / (max ? max : 1);
    }

    for (uint16_t i = 0; i < sc; i++) {
        color_t color = canvas->pal->fg;

        if (min + max) { // Non-symmetric scale
            if (i >= value) {
                color = canvas->pal->bg;
            }
        } else { // Symmetric scale
            if ((value > 0 && i >= value + (sc / 2)) ||
                (value >= 0 && i < (sc / 2 - 1)) ||
                (value < 0 && i < value + (sc / 2)) ||
                (value <= 0 && i > (sc / 2))) {
                color = canvas->pal->bg;
            }
        }

        glcdDrawRect(i * (width / sc), barPos, sw, barHalf, color);
        glcdDrawRect(i * (width / sc), barPos + barHalf, sw, barMiddle, canvas->pal->fg);
        glcdDrawRect(i * (width / sc), barPos + barHalf + barMiddle, sw, barHalf, color);
    }
}
