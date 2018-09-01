#include "glcd.h"

#include "../pins.h"

#define STR_BUFSIZE             20

static char strbuf[STR_BUFSIZE + 1];   // String buffer

static GlcdDriver *glcd;
static CharParam charParam;

void glcdInit(GlcdDriver *driver)
{
    glcd = driver;
}

void glcdClear(void)
{
    if (glcd->clear) {
        glcd->clear();
    }
}

GlcdCanvas *glcdGetCanvas(void)
{
    return glcd->canvas;
}

uint8_t glcdGetBus(void)
{
    return glcd->bus;
}

void glcdWriteNum(int32_t number, uint8_t width, uint8_t lead, uint8_t radix)
{
    uint8_t numdiv;
    uint8_t sign = lead;
    int8_t i;
    uint32_t num = (uint32_t)number;

    if (number < 0 && radix == 10) {
        sign = '-';
        num = -number;
    }

    for (i = 0; i < width; i++)
        strbuf[i] = lead;
    strbuf[width] = '\0';
    i = width - 1;

    while (num > 0 || i == width - 1) {
        numdiv = num % radix;
        strbuf[i] = numdiv + 0x30;
        if (numdiv >= 10)
            strbuf[i] += 7;
        i--;
        num /= radix;
    }

    if (i >= 0)
        strbuf[i] = sign;

    glcdWriteString(strbuf);
}

void glcdLoadFont(const uint8_t *font)
{
    glcd->font.data = font;
}

void glcdSetFontColor(uint16_t color)
{
    glcd->font.color = color;
}

void glcdSetFontMult(uint8_t mult)
{
    glcd->font.mult = mult;
}

void glcdSetCanvasColor(uint16_t color)
{
    glcd->canvas->color = color;
}

void glcdSetXY(int16_t x, int16_t y)
{
    glcd->canvas->x = x;
    glcd->canvas->y = y;
}

void glcdSetX(int16_t x)
{
    glcd->canvas->x = x;
}

static void findCharOft(uint8_t code)
{
    uint8_t i;
    const uint8_t *fp = glcd->font.data;

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
    uint8_t h = glcd->font.data[FONT_HEIGHT];
    uint16_t color = glcd->font.color;
    uint16_t bgColor = glcd->canvas->color;
    uint8_t mult = glcd->font.mult;

    for (uint16_t j = 0; j < h; j++) {
        for (uint16_t i = 0; i < w; i++) {
            uint8_t data = param->data[w * j + i];
            for (uint8_t k = 0; k < 8; k++) {
                for (uint8_t mx = 0; mx < mult; mx++) {
                    for (uint8_t my = 0; my < mult; my++) {
                        glcd->drawPixel(glcd->canvas->x + mult * i + mx, glcd->canvas->y + mult * (8 * j + k) + my,
                                        data & (1 << k) ? color : bgColor);
                    }
                }
            }
        }
    }
}

void glcdWriteChar(uint8_t code)
{
    findCharOft(code);
    glcd->drawFontChar(&charParam);
    glcdSetX(glcd->canvas->x + charParam.width * glcd->font.mult);
}

void glcdWriteString(char *string)
{
    if (*string)
        glcdWriteChar(*string++);
    while (*string) {
        glcdWriteChar(glcd->font.data[FONT_LTSPPOS]);
        glcdWriteChar(*string++);
    }
}

void glcdDrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (glcd->drawRectangle) {
        glcd->drawRectangle(x, y, w, h, color);
    } else {
        // TODO: draw in pixels
    }
}

void glcdDrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    if (x0 == x1) {                 // Vertical
        if (y0 > y1) {              // Swap
            y0 = y0 + y1;
            y1 = y0 - y1;
            y0 = y0 - y1;
        }
        glcdDrawRect(x0, y0, 1, y1 - y0 + 1, color);
    } else if (y0 == y1) {          // Horisontal
        if (x0 > x1) {              // Swap
            x0 = x0 + x1;
            x1 = x0 - x1;
            x0 = x0 - x1;
        }
        glcdDrawRect(x0, y0, x1 - x0 + 1, 1, color);
    } else {
        int16_t sX, sY, dX, dY, err, err2;

        sX = x0 < x1 ? 1 : -1;
        sY = y0 < y1 ? 1 : -1;
        dX = sX > 0 ? x1 - x0 : x0 - x1;
        dY = sY > 0 ? y1 - y0 : y0 - y1;
        err = dX - dY;

        while (x0 != x1 || y0 != y1) {
            glcd->drawPixel(x0, y0, color);
            err2 = err * 2;
            if (err2 > -dY / 2) {
                err -= dY;
                x0 += sX;
            }
            if (err2 < dX) {
                err += dX;
                y0 += sY;
            }
        }
        glcd->drawPixel(x1, y1, color);
    }
}

void glcdDrawFrame(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    glcdDrawLine(x0, y0, x0, y1, color);
    glcdDrawLine(x0, y1, x1, y1, color);
    glcdDrawLine(x1, y0, x1, y1, color);
    glcdDrawLine(x0, y0, x1, y0, color);
}

void glcdDrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    glcdDrawLine(x0 - r, y0, x0 + r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        glcdDrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        glcdDrawLine(x0 - x, y0 - y, x0 + x, y0 - y, color);
        glcdDrawLine(x0 - y, y0 + x, x0 + y, y0 + x, color);
        glcdDrawLine(x0 - y, y0 - x, x0 + y, y0 - x, color);
    }
}

void glcdDrawRing(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    glcd->drawPixel(x0, y0 + r, color);
    glcd->drawPixel(x0, y0 - r, color);
    glcd->drawPixel(x0 + r, y0, color);
    glcd->drawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        glcd->drawPixel(x0 + x, y0 + y, color);
        glcd->drawPixel(x0 - x, y0 + y, color);
        glcd->drawPixel(x0 + x, y0 - y, color);
        glcd->drawPixel(x0 - x, y0 - y, color);

        glcd->drawPixel(x0 + y, y0 + x, color);
        glcd->drawPixel(x0 - y, y0 + x, color);
        glcd->drawPixel(x0 + y, y0 - x, color);
        glcd->drawPixel(x0 - y, y0 - x, color);
    }
}

void glcdUpdate(void)
{
    if (glcd->updateFB) {
        glcd->updateFB();
    }
}
