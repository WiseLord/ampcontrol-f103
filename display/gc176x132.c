#include "gc176x132.h"

#include "fonts.h"
#include "icons.h"

#include "../functions.h"

static GlcdDriver *glcd;

static void displayTm(RTC_type *rtc, uint8_t tm)
{
    char ltSp = glcd->font.data[FONT_LTSPPOS];
    int8_t time = *((int8_t *)rtc + tm);

    glcdSetFontColor(LCD_COLOR_AQUA);
    if (rtc->etm == tm)
        glcdSetFontColor(LCD_COLOR_YELLOW);
    glcdWriteChar(ltSp);
    if (tm == RTC_YEAR) {
        glcdWriteString("20");
        glcdWriteChar(ltSp);
    }
    glcdWriteNum(time, 2, '0', 10);
    glcdWriteChar(ltSp);
    glcdSetFontColor(LCD_COLOR_AQUA);
    glcdWriteChar(ltSp);
}

static void drawSpCol(uint16_t xbase, uint16_t ybase, uint8_t width, uint16_t value, uint16_t max)
{
    if (value > max)
        value = max;

    glcd->drawRectangle(xbase, ybase - value, width, value, LCD_COLOR_AQUA);
    glcd->drawRectangle(xbase, ybase - max, width, max - value, LCD_COLOR_BLACK);
}

static void showTime(RTC_type *rtc, char *wday)
{
    glcdSetXY(2, 4);
    glcdSetFontMult(2);
    glcdLoadFont(font_ks0066_ru_24);

    displayTm(rtc, RTC_HOUR);
    glcdWriteChar(':');
    displayTm(rtc, RTC_MIN);
    glcdWriteChar(':');
    displayTm(rtc, RTC_SEC);

    glcdSetXY(12, 64);
    glcdSetFontMult(1);
    glcdLoadFont(font_digits_32);

    displayTm(rtc, RTC_DATE);
    glcdWriteChar('.');
    displayTm(rtc, RTC_MONTH);
    glcdWriteChar('.');
    displayTm(rtc, RTC_YEAR);

    glcdLoadFont(font_ks0066_ru_24);
    glcdSetFontColor(LCD_COLOR_AQUA);
    glcdSetXY(24, 104);
    glcdWriteString(wday);
}

static void showParam(DispParam *dp)
{
    glcd->drawRectangle(10, 10, 50, 30, LCD_COLOR_GREEN);
}

static void showSpectrum(uint8_t *dataL, uint8_t *dataR)
{
    for (uint16_t x = 0; x < (glcd->canvas->width + 1) / 2; x++) {
        uint16_t xbase = x * 2;
        uint16_t ybase = 66;
        uint16_t width = 1;
        uint16_t value = dataL[x];
        uint16_t max = 65;

        drawSpCol(xbase, ybase, width, value + 1, max);
    }
    for (uint16_t x = 0; x < (glcd->canvas->width + 1) / 2; x++) {
        uint16_t xbase = x * 2;
        uint16_t ybase = 132;
        uint16_t width = 1;
        uint16_t value = dataR[x];
        uint16_t max = 65;

        drawSpCol(xbase, ybase, width, value + 1, max);
    }
}

GlcdCanvas gc176x132 = {
    .width = 176,
    .height = 132,

    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
};

void gc176x132Init(GlcdDriver *driver)
{
    glcd = driver;
    glcd->canvas = &gc176x132;
    glcdInit(glcd);

    glcdSetFontMult(2);
}
