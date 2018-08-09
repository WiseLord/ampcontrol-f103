#include "glcd.h"

#define STR_BUFSIZE             20

static char strbuf[STR_BUFSIZE + 1];   // String buffer

DisplayDriver *disp;

const uint8_t *_font;
static uint8_t fp[FONT_PARAM_END];

void glcdInit(DisplayDriver *driver)
{
    disp = driver;
}

void glcdWriteNum(int16_t number, uint8_t width, uint8_t lead, uint8_t radix)
{
    uint8_t numdiv;
    uint8_t sign = lead;
    int8_t i;

    if (number < 0) {
        sign = '-';
        number = -number;
    }

    for (i = 0; i < width; i++)
        strbuf[i] = lead;
    strbuf[width] = '\0';
    i = width - 1;

    while (number > 0 || i == width - 1) {
        numdiv = number % radix;
        strbuf[i] = numdiv + 0x30;
        if (numdiv >= 10)
            strbuf[i] += 7;
        i--;
        number /= radix;
    }

    if (i >= 0)
        strbuf[i] = sign;

    glcdWriteString(strbuf);
}

void glcdLoadFont(const uint8_t *font, uint16_t color, uint8_t direction)
{
    uint8_t i;

    _font = font + 5;
    for (i = 0; i < FONT_END - 1; i++)
        fp[i] = font[i];
    fp[FONT_COLOR] = color;
    fp[FONT_DIRECTION] = direction;
    fp[FONT_FIXED] = 0;
}

void glcdSetXY(int16_t x, int16_t y)
{
    disp->layout->x = x;
    disp->layout->y = y;
}

void glcdWriteChar(uint8_t code)
{
    uint8_t i;
    uint8_t j;
    uint8_t k;

    uint8_t pgmData;

    uint8_t spos = code - ((code >= 128) ? fp[FONT_OFTNA] : fp[FONT_OFTA]);

    uint16_t oft = 0;               // Current symbol offset in array
    uint8_t swd = 0;                // Current symbol width
    uint8_t fwd = fp[FONT_FIXED];   // Fixed width

    for (i = 0; i < spos; i++) {
        swd = _font[i];
        oft += swd;
    }
    swd = _font[spos];
    if (!fwd)
        fwd = swd;

    oft *= fp[FONT_HEIGHT];
    oft += fp[FONT_CCNT];

    for (j = 0; j < fp[FONT_HEIGHT]; j++) {
        for (i = 0; i < fwd; i++) {
            if (i >= swd)
                pgmData = 0x00;
            else
                pgmData = _font[oft + (swd * j) + i];
            if (!fp[FONT_COLOR])
                pgmData = ~pgmData;
            for (k = 0; k < 8; k++) {
                switch (fp[FONT_DIRECTION]) {
                case FONT_DIR_0:
                    disp->drawPixel(disp->layout->x + i, disp->layout->y + (8 * j + k), pgmData & (1 << k));
                    break;
                case FONT_DIR_90:
                    disp->drawPixel(disp->layout->x + (8 * j + k), disp->layout->y - i, pgmData & (1 << k));
                    break;
                case FONT_DIR_180:
                    disp->drawPixel(disp->layout->x - i, disp->layout->y - (8 * j + k), pgmData & (1 << k));
                    break;
                case FONT_DIR_270:
                    disp->drawPixel(disp->layout->x - (8 * j + k), disp->layout->y + i, pgmData & (1 << k));
                    break;
                }
            }
        }
    }
    switch (fp[FONT_DIRECTION]) {
    case FONT_DIR_0:
        glcdSetXY(disp->layout->x + fwd, disp->layout->y);
        break;
    case FONT_DIR_90:
        glcdSetXY(disp->layout->x, disp->layout->y - fwd);
        break;
    case FONT_DIR_180:
        glcdSetXY(disp->layout->x - fwd, disp->layout->y);
        break;
    case FONT_DIR_270:
        glcdSetXY(disp->layout->x, disp->layout->y + fwd);
        break;
    }
}

void glcdWriteString(char *string)
{
    if (*string)
        glcdWriteChar(*string++);
    while (*string) {
        if (!fp[FONT_FIXED])
            glcdWriteChar(fp[FONT_LTSPPOS]);
        glcdWriteChar(*string++);
    }
}

