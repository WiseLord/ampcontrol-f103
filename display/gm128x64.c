#include "gm128x64.h"

#include "gdfb.h"

static void showTime(RTC_type *rtc, char *wday);
static void showParam(DispParam *dp);
static void showSpectrum(uint8_t *dataL, uint8_t *dataR);

Display gm128x64 = {
    .width = 128,
    .height = 64,
    .writeChar = gdWriteChar,
    .writeString = gdWriteString,
    .setBrightness = ks0108SetBrightness,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
};

static void drawSpCol(uint8_t xbase, uint8_t w, uint8_t btm, uint8_t val, uint8_t max)
{
    uint8_t i;

    val = (val < max ? btm - val : btm - max);

    for (i = 0; i < w; i++) {
        gdDrawVertLine(xbase + i, btm, val, 1);
        gdDrawVertLine(xbase + i, val > (btm - max) ? val - 1 : val, btm - max, 0);
    }
}

static void displayTm(RTC_type *rtc, uint8_t tm, const uint8_t *font)
{
    char ltSp = font[FONT_LTSPPOS];
    int8_t time = *((int8_t *)rtc + tm);

    gdLoadFont(font, 1, FONT_DIR_0);
    gm128x64.writeChar(ltSp);
    gdLoadFont(font, rtc->etm == tm ? 0 : 1, FONT_DIR_0);
    gm128x64.writeChar(ltSp);
    if (tm == RTC_YEAR) {
        gm128x64.writeString("20");
        gm128x64.writeChar(ltSp);
    }
    displayWriteNum(time, 2, '0', 10);
    gm128x64.writeChar(ltSp);
    gdLoadFont(font, 1, FONT_DIR_0);
    gm128x64.writeChar(ltSp);
}

static void showTime(RTC_type *rtc, char *wday)
{
    gdSetXY(4, 0);

    displayTm(rtc, RTC_HOUR, font_digits_32);
    gm128x64.writeChar(':');
    displayTm(rtc, RTC_MIN, font_digits_32);
    gm128x64.writeChar(':');
    displayTm(rtc, RTC_SEC, font_digits_32);

    gdSetXY(5, 32);

    displayTm(rtc, RTC_DATE, font_ks0066_ru_24);
    gm128x64.writeChar('.');
    displayTm(rtc, RTC_MONTH, font_ks0066_ru_24);
    gm128x64.writeChar('.');
    displayTm(rtc, RTC_YEAR, font_ks0066_ru_24);

    gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
    gdSetXY(36, 56);
    gm128x64.writeString(wday);
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
                    gdDrawPixel(i, j, 1);
                } else {
                    gdDrawPixel(i, j, color);
                }
            }
        }
    }
}

static void displayShowIcon(uint8_t icon)
{
    if (icon < ICON24_END) {
        gdSetXY(104, 2);
        gdWriteIcon24(icon);
    }
}


static void showParam(DispParam *dp)
{
    gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
    gdSetXY(0, 0);
    gdWriteString((char *)dp->label);

    displayShowBar(dp->min, dp->max, dp->value);

    gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
    gdSetXY(94, 30);
    displayWriteNum(dp->value, 3, ' ', 10);

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

void gm128x64Init(Display **disp)
{
    *disp = &gm128x64;

    gdInit();
    gdClear();
}
