#include "gm128x64.h"

#include "fonts.h"
#include "icons.h"

static void showTime(RTC_type *rtc, char *wday);
static void showParam(DispParam *dp);
static void showSpectrum(uint8_t *dataL, uint8_t *dataR);

DisplayDriver *disp;

DisplayLayout gm128x64 = {
    .width = 128,
    .height = 64,

    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
};

void drawHorizLine(uint8_t x1, uint8_t x2, uint8_t y, uint8_t color)
{
    uint8_t i;

    // Swap X
    if (x1 > x2) {
        i = x1;
        x1 = x2;
        x2 = i;
    }

    for (i = x1; i <= x2; i++)
        disp->drawPixel(i, y, color);
}

void drawVertLine(uint8_t x, uint8_t y1, uint8_t y2, uint8_t color)
{
    uint8_t i;

    // Swap Y
    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;
    }

    for (i = y1; i <= y2; i++)
        disp->drawPixel(x, i, color);
}


void drawIcon24(uint8_t iconNum)
{
    uint8_t i, j, k;
    uint8_t pgmData;

    const uint8_t *icon;

    icon = &icons_24[24 * 24 / 8 * iconNum];

    if (icon) {
        for (j = 0; j < 3; j++) {
            for (i = 0; i < 24; i++) {
                pgmData = icon[24 * j + i];
                for (k = 0; k < 8; k++) {
                    disp->drawPixel(disp->layout->x + i, disp->layout->y + 8 * j + k, pgmData & (1 << k));
                }
            }
        }
    }
}

void drawIcon32(uint8_t iconNum)
{
    uint8_t i, j, k;
    uint8_t pgmData;

    const uint8_t *icon;

    icon = &icons_32[32 * 32 / 8 * iconNum];

    if (icon) {
        for (j = 0; j < 4; j++) {
            for (i = 0; i < 32; i++) {
                pgmData = icon[32 * j + i];
                for (k = 0; k < 8; k++) {
                    disp->drawPixel(disp->layout->x + i, disp->layout->y + 8 * j + k, pgmData & (1 << k));
                }
            }
        }
    }
}

static void drawSpCol(uint8_t xbase, uint8_t w, uint8_t btm, uint8_t val, uint8_t max)
{
    uint8_t i;

    val = (val < max ? btm - val : btm - max);

    for (i = 0; i < w; i++) {
        drawVertLine(xbase + i, btm, val, 1);
        drawVertLine(xbase + i, val > (btm - max) ? val - 1 : val, btm - max, 0);
    }
}

/*static*/ void displayTm(RTC_type *rtc, uint8_t tm, const uint8_t *font)
{
    char ltSp = font[FONT_LTSPPOS];
    int8_t time = *((int8_t *)rtc + tm);

    glcdLoadFont(font, 1, FONT_DIR_0);
    glcdWriteChar(ltSp);
    glcdLoadFont(font, rtc->etm == tm ? 0 : 1, FONT_DIR_0);
    glcdWriteChar(ltSp);
    if (tm == RTC_YEAR) {
        glcdWriteString("20");
        glcdWriteChar(ltSp);
    }
    glcdWriteNum(time, 2, '0', 10);
    glcdWriteChar(ltSp);
    glcdLoadFont(font, 1, FONT_DIR_0);
    glcdWriteChar(ltSp);
}

static void showTime(RTC_type *rtc, char *wday)
{
    glcdSetXY(4, 0);

    displayTm(rtc, RTC_HOUR, font_digits_32);
    glcdWriteChar(':');
    displayTm(rtc, RTC_MIN, font_digits_32);
    glcdWriteChar(':');
    displayTm(rtc, RTC_SEC, font_digits_32);

    glcdSetXY(5, 32);

    displayTm(rtc, RTC_DATE, font_ks0066_ru_24);
    glcdWriteChar('.');
    displayTm(rtc, RTC_MONTH, font_ks0066_ru_24);
    glcdWriteChar('.');
    displayTm(rtc, RTC_YEAR, font_ks0066_ru_24);

    glcdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
    glcdSetXY(36, 56);
    glcdWriteString(wday);
}

static void displayShowBar(int16_t min, int16_t max, int16_t value)
{
    uint8_t i, j;
    uint8_t color;

    if (min + max) {
        value = (uint16_t)91 * (value - min) / (max - min);
    } else {
        value = (int16_t)45 * value / max;
    }

    for (i = 0; i < 91; i++) {
        if (((min + max) && (value <= i)) || (!(min + max) &&
                                              (((value > 0) && ((i < 45) || (value + 45 < i))) ||
                                               ((value <= 0) && ((i > 45) || (value + 45 > i)))))) {
            color = 0x00;
        } else {
            color = 0x01;
        }
        if (!(i & 0x01)) {
            for (j = 27; j < 38; j++) {
                if (j == 32) {
                    disp->drawPixel(i, j, 1);
                } else {
                    disp->drawPixel(i, j, color);
                }
            }
        }
    }
}

static void displayShowIcon(uint8_t icon)
{
    if (icon < ICON24_END) {
        glcdSetXY(104, 2);
        drawIcon24(icon);
    }
}


static void showParam(DispParam *dp)
{
    glcdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
    glcdSetXY(0, 0);
    glcdWriteString((char *)dp->label);

    displayShowBar(dp->min, dp->max, dp->value);

    glcdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
    glcdSetXY(94, 30);
    glcdWriteNum(dp->value, 3, ' ', 10);

    displayShowIcon(dp->icon);
}

static void showSpectrum(uint8_t *dataL, uint8_t *dataR)
{
    uint8_t x, xbase;
    uint8_t y, ybase;
    uint8_t *buf;

    buf = dataL;
    for (x = 0; x < gm128x64.width; x++) {
        xbase = x;
        y = 0;

        ybase = buf[x] / 4;
        drawSpCol(xbase, 1, 31 + y, ybase, 31);
    }

    buf = dataR;
    for (x = 0; x < gm128x64.width; x++) {
        xbase = x;
        y = 32;

        ybase = buf[x] / 4;
        drawSpCol(xbase, 1, 31 + y, ybase, 31);
    }
}

void gm128x64Init(DisplayDriver *driver)
{
    disp = driver;
    disp->layout = &gm128x64;
    glcdInit(disp);
}
