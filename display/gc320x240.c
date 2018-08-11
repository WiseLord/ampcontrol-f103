#include "gc320x240.h"

#include "fonts.h"
#include "icons.h"

DisplayDriver *disp;

static void displayTm(RTC_type *rtc, uint8_t tm)
{
    char ltSp = disp->font.data[FONT_LTSPPOS];
    int8_t time = *((int8_t *)rtc + tm);

    glcdSetFontColor(LCD_COLOR_AQUA);
    glcdWriteChar(ltSp);
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

    disp->drawRectangle(xbase, ybase - value, width, value, LCD_COLOR_AQUA);
    disp->drawRectangle(xbase, ybase - max, width, max - value, LCD_COLOR_BLACK);
}

static void showTime(RTC_type *rtc, char *wday)
{
    glcdSetXY(42, 8);

    glcdLoadFont(font_digits_32);

    displayTm(rtc, RTC_HOUR);
    glcdWriteChar(':');
    displayTm(rtc, RTC_MIN);
    glcdWriteChar(':');
    displayTm(rtc, RTC_SEC);

    glcdSetXY(12, 84);

    displayTm(rtc, RTC_DATE);
    glcdWriteChar('.');
    displayTm(rtc, RTC_MONTH);
    glcdWriteChar('.');
    displayTm(rtc, RTC_YEAR);

    glcdLoadFont(font_ks0066_ru_24);
    glcdSetFontColor(LCD_COLOR_AQUA);
    glcdSetXY(48, 160);
    glcdWriteString(wday);
}

static void showParam(DispParam *dp)
{
    disp->drawRectangle(10, 10, 50, 30, LCD_COLOR_GREEN);
}

static void showSpectrum(uint8_t *dataL, uint8_t *dataR)
{
    for (uint16_t x = 0; x < (disp->layout->width + 1) / 3; x++) {
        uint16_t xbase = x * 3;
        uint16_t ybase = 120;
        uint16_t width = 2;
        uint16_t value = dataL[x];
        uint16_t max = 119;

        drawSpCol(xbase, ybase, width, value + 1, max);
    }
    for (uint16_t x = 0; x < (disp->layout->width + 1) / 3; x++) {
        uint16_t xbase = x * 3;
        uint16_t ybase = 240;
        uint16_t width = 2;
        uint16_t value = dataR[x];
        uint16_t max = 119;

        drawSpCol(xbase, ybase, width, value + 1, max);
    }
}

DisplayLayout gc320x240 = {
    .width = 320,
    .height = 240,

    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
};

void gc320x240Init(DisplayDriver *driver)
{
    disp = driver;
    disp->layout = &gc320x240;
    glcdInit(disp);

    glcdSetFontMult(2);
}
