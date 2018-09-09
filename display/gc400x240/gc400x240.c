#include "gc400x240.h"

static GlcdDriver *glcd;

static void displayTm(RTC_type *rtc, uint8_t tm)
{
    int8_t time = *((int8_t *)rtc + tm);

    glcdSetFontColor(LCD_COLOR_AQUA);
    glcdWriteChar(LETTER_SPACE_CHAR);
    if (rtc->etm == tm)
        glcdSetFontColor(LCD_COLOR_YELLOW);
    glcdWriteChar(LETTER_SPACE_CHAR);
    if (tm == RTC_YEAR) {
        glcdWriteString("20");
        glcdWriteChar(LETTER_SPACE_CHAR);
    }
    glcdWriteNum(time, 2, '0', 10);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdSetFontColor(LCD_COLOR_AQUA);
    glcdWriteChar(LETTER_SPACE_CHAR);
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
    glcdSetXY(82, 20);

    glcdSetFont(&fontampdig32);

    displayTm(rtc, RTC_HOUR);
    glcdWriteChar(':');
    displayTm(rtc, RTC_MIN);
    glcdWriteChar(':');
    displayTm(rtc, RTC_SEC);

    glcdSetXY(52, 96);

    displayTm(rtc, RTC_DATE);
    glcdWriteChar('.');
    displayTm(rtc, RTC_MONTH);
    glcdWriteChar('.');
    displayTm(rtc, RTC_YEAR);

    glcdSetFont(&fontamp24);
    glcdSetFontColor(LCD_COLOR_AQUA);
    glcdSetXY(88, 172);
    glcdWriteString(wday);
}

static void showParam(DispParam *dp)
{
    glcd->drawRectangle(10, 10, 50, 30, LCD_COLOR_GREEN);
}

static void showSpectrum(SpectrumData *spData)
{
    uint8_t *buf;

    buf = spData[SP_CHAN_LEFT].show;
    for (uint16_t x = 0; x < (glcd->canvas->width - 16) / 3; x++) {
        uint16_t xbase = x * 4;
        uint16_t ybase = 120;
        uint16_t width = 2;
        uint16_t value = buf[x];
        uint16_t max = 119;

        drawSpCol(xbase, ybase, width, value + 1, max);
    }

    buf = spData[SP_CHAN_RIGHT].show;
    for (uint16_t x = 0; x < (glcd->canvas->width - 16) / 3; x++) {
        uint16_t xbase = x * 4;
        uint16_t ybase = 240;
        uint16_t width = 2;
        uint16_t value = buf[x];
        uint16_t max = 119;

        drawSpCol(xbase, ybase, width, value + 1, max);
    }
}

GlcdCanvas gc400x240 = {
    .width = 400,
    .height = 240,

    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
};

void gc400x240Init(GlcdDriver *driver)
{
    glcd = driver;
    glcd->canvas = &gc400x240;
    glcdInit(glcd);

    glcdSetFontMult(2);
}
