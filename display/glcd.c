#include "glcd.h"

#include "../pins.h"

#define STR_BUFSIZE             20

static char strbuf[STR_BUFSIZE + 1];   // String buffer

DisplayDriver *disp;
CharParam charParam;

static uint8_t brightness;

void glcdInit(DisplayDriver *driver)
{
    disp = driver;
}

void glcdSetBrightness(uint8_t value)
{
    brightness = value;
}

void glcdPWM()
{
    static uint8_t br;

    if (++br >= GLCD_MAX_BRIGHTNESS)
        br = GLCD_MIN_BRIGHTNESS;

    if (br == brightness) {
        CLR(DISP_BCKL);
    } else if (br == 0) {
        SET(DISP_BCKL);
    }
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

void glcdLoadFont(const uint8_t *font)
{
    disp->font.data = font;
}

void glcdSetFontColor(uint16_t color)
{
    disp->font.color = color;
}

void glcdSetFontMult(uint8_t mult)
{
    disp->font.mult = mult;
}

void glcdSetXY(int16_t x, int16_t y)
{
    disp->layout->x = x;
    disp->layout->y = y;
}

void glcdSetX(int16_t x)
{
    disp->layout->x = x;
}

/*static*/ void findCharOft(uint8_t code)
{
    uint8_t i;
    const uint8_t *fp = disp->font.data;

    uint8_t spos = code - ((code >= 128) ? fp[FONT_OFTNA] : fp[FONT_OFTA]);

    uint16_t oft = 0;               // Current symbol offset in array
    uint8_t swd = 0;                // Current symbol width

    for (i = 0; i < spos; i++) {
        swd = fp[FONT_HEADER_END + i];
        oft += swd;
    }
    swd = fp[FONT_HEADER_END + spos];

    oft *= fp[FONT_HEIGHT];
    oft += FONT_HEADER_END;
    oft += fp[FONT_CCNT];

    charParam.data = fp + oft;
    charParam.width = swd;
}

void glcdDrawFontChar(CharParam *param)
{
    uint8_t w = param->width;
    uint8_t h = disp->font.data[FONT_HEIGHT];
    uint16_t color = disp->font.color;
    uint16_t bgColor = disp->layout->color;
    uint8_t mult = disp->font.mult;

    for (uint16_t j = 0; j < h; j++) {
        for (uint16_t i = 0; i < w; i++) {
            uint8_t data = param->data[w * j + i];
            if (!color)
                data = ~data;
            for (uint8_t k = 0; k < 8; k++) {
                for (uint8_t mx = 0; mx < mult; mx++) {
                    for (uint8_t my = 0; my < mult; my++) {
                        disp->drawPixel(disp->layout->x + mult * i + mx, disp->layout->y + mult * (8 * j + k) + my, data & (1 << k) ? color : bgColor);
                    }
                }
            }
        }
    }
}

void glcdWriteChar(uint8_t code)
{
    findCharOft(code);
    disp->drawFontChar(&charParam);
    glcdSetX(disp->layout->x + charParam.width * disp->font.mult);
}

void glcdWriteString(char *string)
{
    if (*string)
        glcdWriteChar(*string++);
    while (*string) {
        glcdWriteChar(disp->font.data[FONT_LTSPPOS]);
        glcdWriteChar(*string++);
    }
}

