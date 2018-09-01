#include "gm128x64.h"

#include "fonts.h"
#include "icons.h"

static GlcdDriver *glcd;

//static void drawHorizLine(uint8_t x1, uint8_t x2, uint8_t y, uint8_t color)
//{
//    uint8_t i;

//    // Swap X
//    if (x1 > x2) {
//        i = x1;
//        x1 = x2;
//        x2 = i;
//    }

//    for (i = x1; i <= x2; i++)
//        disp->drawPixel(i, y, color);
//}

static void drawVertLine(uint8_t x, uint8_t y1, uint8_t y2, uint8_t color)
{
    uint8_t i;

    // Swap Y
    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;
    }

    for (i = y1; i <= y2; i++)
        glcd->drawPixel(x, i, color);
}

// TODO: move to glcd and make work with rectangle area
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
                    glcd->drawPixel(glcd->canvas->x + i, glcd->canvas->y + 8 * j + k, pgmData & (1 << k));
                }
            }
        }
    }
}

// TODO: move to glcd and make work with rectangle area
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
                    glcd->drawPixel(glcd->canvas->x + i, glcd->canvas->y + 8 * j + k, pgmData & (1 << k));
                }
            }
        }
    }
}

static void displayTm(RTC_type *rtc, uint8_t tm)
{
    char ltSp = glcd->font.data[FONT_LTSPPOS];
    int8_t time = *((int8_t *)rtc + tm);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetCanvasColor(LCD_COLOR_BLACK);
    glcdWriteChar(ltSp);
    if (rtc->etm == tm) {
        glcdSetFontColor(LCD_COLOR_BLACK);
        glcdSetCanvasColor(LCD_COLOR_WHITE);
    }
    glcdWriteChar(ltSp);
    if (tm == RTC_YEAR) {
        glcdWriteString("20");
        glcdWriteChar(ltSp);
    }
    glcdWriteNum(time, 2, '0', 10);
    glcdWriteChar(ltSp);
    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetCanvasColor(LCD_COLOR_BLACK);
    glcdWriteChar(ltSp);
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
                    glcd->drawPixel(i, j, 1);
                } else {
                    glcd->drawPixel(i, j, color);
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

static void drawSpCol(uint8_t xbase, uint8_t w, uint8_t btm, uint8_t val, uint8_t max)
{
    uint8_t i;

    val = (val < max ? btm - val : btm - max);

    for (i = 0; i < w; i++) {
        drawVertLine(xbase + i, btm, val, 1);
        drawVertLine(xbase + i, val > (btm - max) ? val - 1 : val, btm - max, 0);
    }
}

static void showTime(RTC_type *rtc, char *wday)
{
    glcdLoadFont(font_digits_32);
    glcdSetXY(4, 0);
    displayTm(rtc, RTC_HOUR);
    glcdWriteChar(':');
    displayTm(rtc, RTC_MIN);
    glcdWriteChar(':');
    displayTm(rtc, RTC_SEC);

    glcdLoadFont(font_ks0066_ru_24);
    glcdSetXY(5, 32);
    displayTm(rtc, RTC_DATE);
    glcdWriteChar('.');
    displayTm(rtc, RTC_MONTH);
    glcdWriteChar('.');
    displayTm(rtc, RTC_YEAR);

    glcdLoadFont(font_ks0066_ru_08);
    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetXY(36, 56);
    glcdWriteString(wday);
}

static void showParam(DispParam *dp)
{
    glcdLoadFont(font_ks0066_ru_24);
    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetXY(0, 0);
    glcdWriteString((char *)dp->label);

    displayShowBar(dp->min, dp->max, dp->value);

    glcdSetXY(94, 30);
//    glcdWriteNum(dp->value, 3, ' ', 10);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);

    displayShowIcon(dp->icon);
}

static void showSpectrum(SpectrumData *spData)
{
    uint8_t x, xbase;
    uint8_t y, ybase;
    uint8_t *buf;

    buf = spData[SP_CHAN_LEFT].show;
    for (x = 0; x < glcd->canvas->width; x++) {
        xbase = x;
        y = 0;

        ybase = buf[x] / 4;
        drawSpCol(xbase, 1, 31 + y, ybase, 31);
    }

    buf = spData[SP_CHAN_RIGHT].show;
    for (x = 0; x < glcd->canvas->width; x++) {
        xbase = x;
        y = 32;

        ybase = buf[x] / 4;
        drawSpCol(xbase, 1, 31 + y, ybase, 31);
    }
}

GlcdCanvas gm128x64 = {
    .width = 128,
    .height = 64,

    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
};

void gm128x64Init(GlcdDriver *driver)
{
    glcd = driver;
    glcd->canvas = &gm128x64;
    glcdInit(glcd);

    glcdSetFontMult(1);
}
