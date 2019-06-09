#include "stripedbar.h"

#include "../../display/glcd.h"

void drawBar(const StripedBar *bar, int16_t value, int16_t min, int16_t max, uint16_t colorFg, uint16_t colorBg)
{
    const int16_t sc = bar->sc;         // Scale count
    const uint8_t sw = bar->sw;         // Scale width
    const int16_t barPos = bar->barY;
    const uint8_t barHalf = bar->half;
    const uint8_t barMiddle = bar->middle;
    const int16_t width = bar->barW;

    if (min + max) { // Non-symmectic scale => rescale to 0..sc
        value = sc * (value - min) / (max - min);
    } else { // Symmetric scale => rescale to -sc/2..sc/2
        value = (sc / 2) * value / (max ? max : 1);
    }

    for (uint16_t i = 0; i < sc; i++) {
        uint16_t color = colorFg;

        if (min + max) { // Non-symmetric scale
            if (i >= value) {
                color = colorBg;
            }
        } else { // Symmetric scale
            if ((value > 0 && i >= value + (sc / 2)) ||
                (value >= 0 && i < (sc / 2 - 1)) ||
                (value < 0 && i < value + (sc / 2)) ||
                (value <= 0 && i > (sc / 2))) {
                color = colorBg;
            }
        }

        glcdDrawRect(i * (width / sc), barPos, sw, barHalf, color);
        glcdDrawRect(i * (width / sc), barPos + barHalf, sw, barMiddle, colorFg);
        glcdDrawRect(i * (width / sc), barPos + barHalf + barMiddle, sw, barHalf, color);
    }
}
