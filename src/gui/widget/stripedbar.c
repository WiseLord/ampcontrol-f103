#include "stripedbar.h"

#include "display/glcd.h"
#include "gui/palette.h"

void stripedBarDraw(bool clear, StripedBar *bar, LayoutStripedBar *lt)
{
    (void)clear;

    const Palette *pal = paletteGet();

    const uint8_t sw = lt->sw;         // Scale width
    const int16_t sc = lt->barW / lt->sw / 2;
    const int16_t width = sc * 2 * sw;

    const int16_t barPos = lt->barY;
    const uint8_t barHalf = lt->half;
    const uint8_t barMiddle = lt->middle;

    int16_t value = bar->value;
    int16_t min = bar->min;
    int16_t max = bar->max;

    if (min + max) { // Non-symmectic scale => rescale to 0..sc
        value = sc * (value - min) / (max - min);
    } else { // Symmetric scale => rescale to -sc/2..sc/2
        value = (sc / 2) * value / (max ? max : 1);
    }

    for (uint16_t i = 0; i < sc; i++) {
        color_t color = pal->fg;

        if (min + max) { // Non-symmetric scale
            if (i >= value) {
                color = pal->bg;
            }
        } else { // Symmetric scale
            if ((value > 0 && i >= value + (sc / 2)) ||
                (value >= 0 && i < (sc / 2 - 1)) ||
                (value < 0 && i < value + (sc / 2)) ||
                (value <= 0 && i > (sc / 2))) {
                color = pal->bg;
            }
        }

        glcdDrawRect(i * (width / sc), barPos, sw, barHalf, color);
        glcdDrawRect(i * (width / sc), barPos + barHalf, sw, barMiddle, pal->fg);
        glcdDrawRect(i * (width / sc), barPos + barHalf + barMiddle, sw, barHalf, color);
    }
}
