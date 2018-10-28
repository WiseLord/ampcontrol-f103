#include "canvas.h"

// On 220x176 we can draw max 4 menu items + menu header
#define MENU_SIZE_VISIBLE   4

static void showTime(RTC_type *rtc, char *wday);
static void showParam(DispParam *dp);
static void showSpectrum(SpectrumData *spData);
//static void showTuner(DispTuner *dt);
//static void showMenu(void);

static Canvas canvas = {
    .width = 220,
    .height = 176,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
    //.showTuner = showTuner,
    //.showMenu = showMenu,
};

void gc220x176Init(Canvas **value)
{
    *value = &canvas;
}

static void displayTm(RTC_type *rtc, uint8_t tm)
{
    int8_t time = *((int8_t *)rtc + tm);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdWriteChar(LETTER_SPACE_CHAR);
    if (rtc->etm == tm)
        glcdSetFontColor(LCD_COLOR_OLIVE);
    if (tm == RTC_YEAR) {
        glcdWriteString("20");
        glcdWriteChar(LETTER_SPACE_CHAR);
    }
    glcdWriteNum(time, 2, '0', 10);
    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdWriteChar(LETTER_SPACE_CHAR);
}

static void drawSpCol(uint16_t xbase, uint16_t ybase, uint8_t width, uint16_t value, uint16_t max)
{
    if (value > max)
        value = max;

    glcdDrawRect(xbase, ybase - value, width, value, LCD_COLOR_AQUA);
    glcdDrawRect(xbase, ybase - max, width, max - value, LCD_COLOR_BLACK);
}

static void showTime(RTC_type *rtc, char *wday)
{
    glcdSetXY(6, 10);
    glcdSetFont(&fontterminusdig58);

    displayTm(rtc, RTC_HOUR);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar(':');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_MIN);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar(':');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_SEC);

    glcdSetXY(10, 78);
    glcdSetFont(&fontterminusdig40);

    displayTm(rtc, RTC_DATE);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_MONTH);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdWriteChar('.');
    glcdWriteChar(LETTER_SPACE_CHAR);
    displayTm(rtc, RTC_YEAR);

    glcdSetXY(109, 126);
    glcdSetFont(&fontterminusmod44);
    glcdSetFontColor(LCD_COLOR_AQUA);

    static char *wdayOld = 0;
    if (wday != wdayOld) {
        glcdDrawRect(0, 124, 220, 32, canvas.color);
    }

    glcdSetFontAlign(FONT_ALIGN_CENTER);
    glcdWriteString(wday);

    wdayOld = wday;
}

static void showParam(DispParam *dp)
{
    glcdSetFont(&fontterminusmod44);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(2, 0);
    glcdWriteString((char *)dp->label);

    glcdSetXY(217, 118);
    glcdSetFont(&fontterminusdig58);

    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);
}

static void showSpectrum(SpectrumData *spData)
{
    uint8_t *buf;

    buf = spData[SP_CHAN_LEFT].show;
    for (uint16_t x = 0; x < (canvas.width + 1) / 3; x++) {
        uint16_t xbase = x * 3;
        uint16_t ybase = 88;
        uint16_t width = 2;
        uint16_t value = buf[x];
        uint16_t max = 87;

        drawSpCol(xbase, ybase, width, value + 1, max);
    }

    buf = spData[SP_CHAN_RIGHT].show;
    for (uint16_t x = 0; x < (canvas.width + 1) / 3; x++) {
        uint16_t xbase = x * 3;
        uint16_t ybase = 176;
        uint16_t width = 2;
        uint16_t value = buf[x];
        uint16_t max = 87;

        drawSpCol(xbase, ybase, width, value + 1, max);
    }
}
