#include "gc320x240.h"

#include "fonts.h"
#include "icons.h"

#include "ili9320.h" // TODO: remove after updating driver

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
    ili9320DrawRectangle(10, 10, 50, 30, LCD_COLOR_GREEN);
}

static void showSpectrum(uint8_t *dataL, uint8_t *dataR)
{
    for (uint16_t i = 0; i < (disp->layout->width + 1) / 3; i++) {
        uint16_t h;
        h = dataL[i];
        if (h > 119)
            h = 119;
        ili9320DrawRectangle(i * 3, 0, 2, h, LCD_COLOR_AQUA);
        ili9320DrawRectangle(i * 3, h, 2, 119 - h, LCD_COLOR_BLACK);
        h = dataR[i];
        if (h > 119)
            h = 119;
        ili9320DrawRectangle(i * 3, 120, 2, h, LCD_COLOR_AQUA);
        ili9320DrawRectangle(i * 3, 120 + h, 2, 119 - h, LCD_COLOR_BLACK);
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
