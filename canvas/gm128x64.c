#include "canvas.h"

static void showTime(RTC_type *rtc, char *wday);
static void showParam(DispParam *dp);
static void showSpectrum(SpectrumData *spData);
//static void showTuner(DispTuner *dt);

static Canvas canvas = {
    .width = 128,
    .height = 644,
    .showTime = showTime,
    .showParam = showParam,
    .showSpectrum = showSpectrum,
//    .showTuner = showTuner,
};

static void displayTm(RTC_type *rtc, uint8_t tm)
{
    int8_t time = *((int8_t *)rtc + tm);

    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(LCD_COLOR_BLACK);
    glcdWriteChar(LETTER_SPACE_CHAR);
    if (rtc->etm == tm) {
        glcdSetFontColor(LCD_COLOR_BLACK);
        glcdSetFontBgColor(LCD_COLOR_WHITE);
    }
    glcdWriteChar(LETTER_SPACE_CHAR);
    if (tm == RTC_YEAR) {
        glcdWriteString("20");
        glcdWriteChar(LETTER_SPACE_CHAR);
    }
    glcdWriteNum(time, 2, '0', 10);
    glcdWriteChar(LETTER_SPACE_CHAR);
    glcdSetFontColor(LCD_COLOR_WHITE);
    glcdSetFontBgColor(LCD_COLOR_BLACK);
    glcdWriteChar(LETTER_SPACE_CHAR);
}

static void displayShowBar(int16_t min, int16_t max, int16_t value)
{
    static const int16_t sc = 46; // Scale count
    static const uint8_t sw = 1; // Scale width

    if (min + max) { // Non-symmectic scale => rescale to 0..sl
        value = sc * (value - min) / (max - min);
    } else { // Symmetric scale => rescale to -sl/2..sl/2
        value = (sc / 2) * value / max;
    }

    for (uint16_t i = 0; i < sc; i++) {
        uint16_t color = LCD_COLOR_WHITE;

        if (min + max) { // Non-symmetric scale
            if (i >= value) {
                color = canvas.color;
            }
        } else { // Symmetric scale
            if ((value > 0 && i >= value + (sc / 2)) ||
                (value >= 0 && i < (sc / 2 - 1)) ||
                (value < 0 && i < value + (sc / 2)) ||
                (value <= 0 && i > (sc / 2))) {
                color = canvas.color;
            }
        }

        uint16_t width = canvas.width;

        glcdDrawRect(i * (width / sc) + 1, 27, sw, 5, color);
        glcdDrawRect(i * (width / sc) + 1, 32, sw, 1, LCD_COLOR_WHITE);
        glcdDrawRect(i * (width / sc) + 1, 33, sw, 5, color);
    }
}

static void drawSpCol(uint8_t xbase, uint8_t ybase, uint8_t width, uint8_t value, uint8_t max, uint8_t peak)
{
    if (value > max)
        value = max;

    if (peak > max - 1)
        peak = max - 1;

    glcdDrawRect(xbase, ybase - value, width, value, LCD_COLOR_AQUA);
    glcdDrawRect(xbase, ybase - max, width, max - value, LCD_COLOR_BLACK);
    glcdDrawRect(xbase, ybase - peak - 1, width, 1, LCD_COLOR_YELLOW);
}

static void showTime(RTC_type *rtc, char *wday)
{
    glcdSetFont(&fontterminusdig30);
    glcdSetXY(1, 0);
    displayTm(rtc, RTC_HOUR);
    glcdWriteChar(':');
    displayTm(rtc, RTC_MIN);
    glcdWriteChar(':');
    displayTm(rtc, RTC_SEC);

    glcdSetXY(1, 30);
    glcdSetFont(&fontterminusdig22);

    displayTm(rtc, RTC_DATE);
    glcdWriteChar('.');
    displayTm(rtc, RTC_MONTH);
    glcdWriteChar('.');
    displayTm(rtc, RTC_YEAR);

    glcdSetXY(63, 52);
    glcdSetFont(&fontterminus12);
    glcdSetFontColor(LCD_COLOR_WHITE);

    static char *wdayOld = 0;
    if (wday != wdayOld) {
        glcdDrawRect(0, 52, 128, 12, canvas.color);
    }

    glcdSetFontAlign(FONT_ALIGN_CENTER);
    glcdWriteString(wday);

    wdayOld = wday;
}

static void showParam(DispParam *dp)
{
    glcdSetFont(&fontterminus24b);
    glcdSetFontColor(LCD_COLOR_WHITE);

    glcdSetXY(0, 0);
    glcdWriteString((char *)dp->label);

    displayShowBar(dp->min, dp->max, dp->value);

    glcdSetXY(128, 30);
    glcdSetFontAlign(FONT_ALIGN_RIGHT);
    glcdWriteNum((dp->value * dp->step) / 8, 3, ' ', 10);

    glcdSetXY(104, 2);
    glcdWriteIcon(dp->icon, icons_24, LCD_COLOR_WHITE, canvas.color);
}

static void showSpectrum(SpectrumData *spData)
{
    uint8_t *buf;
    uint8_t *peak;

    buf = spData[SP_CHAN_LEFT].show;
    peak = spData[SP_CHAN_LEFT].peak;
    for (uint16_t x = 0; x < canvas.width / 2; x++) {
        uint16_t xbase = x * 2;
        uint8_t ybase = 32;
        uint8_t width = 1;
        uint8_t value = buf[x] / 3;
        uint8_t pValue = peak[x] / 3;
        uint8_t max = 32;

        drawSpCol(xbase, ybase, width, value + 1, max, pValue);
    }

    buf = spData[SP_CHAN_RIGHT].show;
    peak = spData[SP_CHAN_RIGHT].peak;
    for (uint16_t x = 0; x < canvas.width / 2; x++) {
        uint16_t xbase = x * 2;
        uint8_t ybase = 64;
        uint8_t width = 1;
        uint8_t value = buf[x] / 3;
        uint8_t pValue = peak[x] / 3;
        uint8_t max = 32;

        drawSpCol(xbase, ybase, width, value + 1, max, pValue);
    }
}

void gm128x64Init(Canvas **value)
{
    *value = &canvas;
}
